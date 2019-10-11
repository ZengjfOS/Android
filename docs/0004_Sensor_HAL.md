# Sensor HAL


## 参考文档

* [Android HIDL 之 hal 进程启动及服务注册过程](https://blog.csdn.net/qq_19923217/article/details/89173162)
* [AndroidHAL-IIO](https://www.st.com/content/st_com/zh/products/embedded-software/mems-and-sensors-software/drivers-for-mems/androidhal-iio.html)
* [iio-tools](https://github.com/ZengjfOS/iio-tools)


## 文件路径

* 原始路径
  * vendor/nxp-opensource/imx/libsensors/Android.mk
    * ifeq ($(BOARD_HAS_SENSOR),true): false
  * vendor/nxp-opensource/imx/libsensors_sensorhub/Android.mk
    * device/fsl/imx8q/mek_8q/BoardConfig.mk
      ```
      # sensor configs
      BOARD_USE_SENSOR_FUSION := true
      BOARD_USE_SENSOR_PEDOMETER := false
      ifeq ($(PRODUCT_IMX_CAR),true)
          BOARD_USE_LEGACY_SENSOR := false
      else
          BOARD_USE_LEGACY_SENSOR :=true
      endif
      ```
* 新路径
  * hardware/STMicroelectronics/SensorHAL_IIO/src/Android.mk
* export PLATFORM_VERSION=9
* export DEFAULT_PLATFORM_VERSION=9 
* make sensors-menuconfig
* mm sensors-defconfig  


## init rc

* /vendor/etc/init/android.hardware.sensors@1.0-service.rc
  ```
  service vendor.sensors-hal-1-0 /vendor/bin/hw/android.hardware.sensors@1.0-service
      class hal
      user system
      group system wakelock
      capabilities BLOCK_SUSPEND
      rlimit rtprio 10 10
  ```


## service hacking

* hardware/interfaces/sensors/1.0
  ```
  Android.bp  default  ISensors.hal  types.hal  vts
  ```
* hardware/interfaces/sensors/1.0/default/service.cpp
  ```
  [...省略]
  #define LOG_TAG "android.hardware.sensors@1.0-service"
  
  #include <android/hardware/sensors/1.0/ISensors.h>
  #include <hidl/LegacySupport.h>
  
  using android::hardware::sensors::V1_0::ISensors;
  using android::hardware::defaultPassthroughServiceImplementation;
  
  int main() {
      /* Sensors framework service needs at least two threads.
       * One thread blocks on a "poll"
       * The second thread is needed for all other HAL methods.
       */
      return defaultPassthroughServiceImplementation<ISensors>(2);
  }
  ```
* system/libhidl/transport/include/hidl/LegacySupport.h
  ```
  [...省略]
  template<class Interface>
  __attribute__((warn_unused_result))
  status_t registerPassthroughServiceImplementation(
          std::string name = "default") {
      sp<Interface> service = Interface::getService(name, true /* getStub */);          // 获取service
  
      if (service == nullptr) {
          ALOGE("Could not get passthrough implementation for %s/%s.",
              Interface::descriptor, name.c_str());
          return EXIT_FAILURE;
      }
  
      LOG_FATAL_IF(service->isRemote(), "Implementation of %s/%s is remote!",
              Interface::descriptor, name.c_str());
  
      status_t status = service->registerAsService(name);
  
      if (status == OK) {
          ALOGI("Registration complete for %s/%s.",
              Interface::descriptor, name.c_str());
      } else {
          ALOGE("Could not register service %s/%s (%d).",
              Interface::descriptor, name.c_str(), status);
      }
  
      return status;
  }

  [...省略]
  template<class Interface>
  __attribute__((warn_unused_result))
  status_t defaultPassthroughServiceImplementation(std::string name,
                                              size_t maxThreads = 1) {
      configureRpcThreadpool(maxThreads, true);
      status_t result = registerPassthroughServiceImplementation<Interface>(name);
  
      if (result != OK) {
          return result;
      }
  
      joinRpcThreadpool();
      return UNKNOWN_ERROR;
  }
  template<class Interface>
  __attribute__((warn_unused_result))
  status_t defaultPassthroughServiceImplementation(size_t maxThreads = 1) {
      return defaultPassthroughServiceImplementation<Interface>("default", maxThreads);
  }
  ```
* out/soong/.intermediates/hardware/interfaces/sensors/1.0/android.hardware.sensors@1.0_genc++/gen/android/hardware/sensors/1.0/SensorsAll.cpp
  ```
  // static
  ::android::sp<ISensors> ISensors::getService(const std::string &serviceName, const bool getStub) {
      return ::android::hardware::details::getServiceInternal<BpHwSensors>(serviceName, true, getStub);
  }
  ```
* details::getServiceInternal 是 libhidl 提供的方法。getServiceInternal 最终调用到 getRawServiceInternal。
* system/libhidl/transport/ServiceManagement.cpp
  * Return<Transport> transportRet = sm->getTransport(descriptor, instance);
* device/fsl/imx8q/mek_8q/manifest.xml


## Sensors.imx.so hacking

* hardware/interfaces/sensors/1.0/ISensors.hal
* hardware/interfaces/sensors/1.0/default/Sensors.cpp
  ```
  Sensors::Sensors()
      : mInitCheck(NO_INIT),
        mSensorModule(nullptr),
        mSensorDevice(nullptr) {
      status_t err = OK;
      if (UseMultiHal()) {
          mSensorModule = ::get_multi_hal_module_info();
      } else {
          err = hw_get_module(
              SENSORS_HARDWARE_MODULE_ID,
              (hw_module_t const **)&mSensorModule);
      }
      if (mSensorModule == NULL) {
          err = UNKNOWN_ERROR;
      }
  
      if (err != OK) {
          LOG(ERROR) << "Couldn't load "
                     << SENSORS_HARDWARE_MODULE_ID
                     << " module ("
                     << strerror(-err)
                     << ")";
  
          mInitCheck = err;
          return;
      }
  
      err = sensors_open_1(&mSensorModule->common, &mSensorDevice);
  
      if (err != OK) {
          LOG(ERROR) << "Couldn't open device for module "
                     << SENSORS_HARDWARE_MODULE_ID
                     << " ("
                     << strerror(-err)
                     << ")";
  
          mInitCheck = err;
          return;
      }
  
      // Require all the old HAL APIs to be present except for injection, which
      // is considered optional.
      CHECK_GE(getHalDeviceVersion(), SENSORS_DEVICE_API_VERSION_1_3);
  
      if (getHalDeviceVersion() == SENSORS_DEVICE_API_VERSION_1_4) {
          if (mSensorDevice->inject_sensor_data == nullptr) {
              LOG(ERROR) << "HAL specifies version 1.4, but does not implement inject_sensor_data()";
          }
          if (mSensorModule->set_operation_mode == nullptr) {
              LOG(ERROR) << "HAL specifies version 1.4, but does not implement set_operation_mode()";
          }
      }
  
      mInitCheck = OK;
  }
  ```
* 上面调用了hw_get_module和sensors_open_1方法，其中SENSORS_HARDWARE_MODULE_ID是在hardware/libhardware/include/hardware/sensors.h中定义的module名字
* hardware/STMicroelectronics/SensorHAL_IIO/src/SensorHAL.cpp
  ```
  /*
   * struct sensors_module_t - Hardware module info
   * see Android documentation.
   */
  struct sensors_module_t HAL_MODULE_INFO_SYM = {
          .common = {
                  .tag = HARDWARE_MODULE_TAG,
                  .module_api_version = SENSORS_MODULE_API_VERSION_0_1,
                  .hal_api_version = 0,
                  .id = SENSORS_HARDWARE_MODULE_ID,
                  .name = "STMicroelectronics Sensors Module",
                  .author = "STMicroelectronics",
                  .methods = &st_hal_sensors_module_methods,
                  .dso = NULL,
                  .reserved = { },
          },
          .get_sensors_list = st_hal_get_sensors_list,
  #if (CONFIG_ST_HAL_ANDROID_VERSION >= ST_HAL_MARSHMALLOW_VERSION)
          .set_operation_mode = st_hal_set_operation_mode,
  #endif /* CONFIG_ST_HAL_ANDROID_VERSION */
  };
  ```


## SensorServer

* open debug: frameworks/native/services/sensorservice/SensorService.h
  * #define DEBUG_CONNECTIONS   true
* logcat error
  ```
  08-16 03:23:25.150  2473  2558 D SystemServerInitThreadPool: Started executing StartSensorService
  08-16 03:23:25.150  2473  2558 D SensorService: nuSensorService starting...
  08-16 03:23:25.151  2260  2260 W hwservicemanager: getTransport: Cannot find entry android.hardware.sensors@1.0::ISensors/default in either framework or device manifest.
  08-16 03:23:25.152  2473  2473 I android_os_HwBinder: HwBinder: Starting thread pool for default::android.hardware.health@2.0::IHealth
  08-16 03:23:25.153  2473  2558 D SystemServerTimingAsync: StartSensorService took to complete: 4ms
  08-16 03:23:25.153  2473  2558 D SystemServerInitThreadPool: Finished executing StartSensorService
  ```
* device/fsl/imx8q/mek_8q/manifest_car.xml: 果然没有配置android.hardware.sensors部分
* 参考device/fsl/imx8q/mek_8q/manifest_car.xml配置device/fsl/imx8q/mek_8q/manifest_car.xml的android.hardware.sensors:
  ```
  <hal format="hidl">
      <name>android.hardware.light</name>
      <transport>hwbinder</transport>
      <version>2.0</version>
      <interface>
          <name>ILight</name>
          <instance>default</instance>
      </interface>
  </hal>
  <hal format="hidl">
     <name>android.hardware.sensors</name>
     <transport>hwbinder</transport>
     <version>1.0</version>
     <interface>
         <name>ISensors</name>
         <instance>default</instance>
     </interface>
  </hal>
  ```
* service list | grep sensor
  * sensorservice: [android.gui.SensorServer]
* frameworks/native/services/sensorservice/SensorService.cpp
  * void SensorService::onFirstRef()
    * SensorDevice& dev(SensorDevice::getInstance());
      ```
      SensorDevice::SensorDevice()
              : mHidlTransportErrors(20), mRestartWaiter(new HidlServiceRegistrationWaiter()) {
          if (!connectHidlService()) {
              return;
          }
      
          [...省略]
      }
      
      bool SensorDevice::connectHidlService() {
          // SensorDevice will wait for HAL service to start if HAL is declared in device manifest.
          size_t retry = 10;
      
          while (retry-- > 0) {
              mSensors = ISensors::getService();
              if (mSensors == nullptr) {
                  // no sensor hidl service found
                  break;
              }
      
              [...省略]
          }
          return (mSensors != nullptr);
      }
      ```
    * ssize_t count = dev.getSensorList(&list);
* frameworks/native/services/sensorservice/SensorService.h
  * 继承：public BinderService<SensorService>,
  * 继承：public BnSensorServer,
  * 继承：protected Thread
* frameworks/base/services/core/jni/com_android_server_SystemServer.cpp
  ```
  static void android_server_SystemServer_startSensorService(JNIEnv* /* env */, jobject /* clazz */) {
      char propBuf[PROPERTY_VALUE_MAX];
      property_get("system_init.startsensorservice", propBuf, "1");
      if (strcmp(propBuf, "1") == 0) {
          SensorService::instantiate();
      }
  
  }

  [...省略]
  /*
   * JNI registration.
   */
  static const JNINativeMethod gMethods[] = {
      /* name, signature, funcPtr */
      { "startSensorService", "()V", (void*) android_server_SystemServer_startSensorService },
      { "startHidlServices", "()V", (void*) android_server_SystemServer_startHidlServices },
  };
  ```
* frameworks/base/services/java/com/android/server/SystemServer.java
  ```
  private void startBootstrapServices() {
      Slog.i(TAG, "Reading configuration...");
  
      [...省略]
  
      // The sensor service needs access to package manager service, app ops
      // service, and permissions service, therefore we start it after them.
      // Start sensor service in a separate thread. Completion should be checked
      // before using it.
      mSensorServiceStart = SystemServerInitThreadPool.get().submit(() -> {
          TimingsTraceLog traceLog = new TimingsTraceLog(
                  SYSTEM_SERVER_TIMING_ASYNC_TAG, Trace.TRACE_TAG_SYSTEM_SERVER);
          traceLog.traceBegin(START_SENSOR_SERVICE);
          startSensorService();
          traceLog.traceEnd();
      }, START_SENSOR_SERVICE);
  }
  ```

## SystemSensorManager 

* frameworks/base/core/java/android/hardware/SystemServiceRegistry.java
  ```
  [...省略]
  final class SystemServiceRegistry {
      private static final String TAG = "SystemServiceRegistry";
          [...省略]
          registerService(Context.SENSOR_SERVICE, SensorManager.class,
                  new CachedServiceFetcher<SensorManager>() {
              @Override
              public SensorManager createService(ContextImpl ctx) {
                  return new SystemSensorManager(ctx.getOuterContext(),
                    ctx.mMainThread.getHandler().getLooper());
              }});
      [...省略]
  }
  [...省略]
  ```
* frameworks/base/core/java/android/hardware/SystemSensorManager.java
  ```
  /** {@hide} */
  public SystemSensorManager(Context context, Looper mainLooper) {
      synchronized (sLock) {
          if (!sNativeClassInited) {
              sNativeClassInited = true;
              nativeClassInit();
          }
      }
  
      mMainLooper = mainLooper;
      mTargetSdkLevel = context.getApplicationInfo().targetSdkVersion;
      mContext = context;
      mNativeInstance = nativeCreate(context.getOpPackageName());
  
      // initialize the sensor list
      for (int index = 0;; ++index) {
          Sensor sensor = new Sensor();
          if (!nativeGetSensorAtIndex(mNativeInstance, sensor, index)) break;
          mFullSensorsList.add(sensor);
          mHandleToSensor.put(sensor.getHandle(), sensor);
      }
  }
  ```
* frameworks/base/core/java/android/app/ContextImpl.java
  ```
  /** @hide */
  @Override
  public String getOpPackageName() {
      return mOpPackageName != null ? mOpPackageName : getBasePackageName();
  }
  ```
* frameworks/base/core/jni/android_hardware_SensorManager.cpp
  ```
  static jlong
  nativeCreate
  (JNIEnv *env, jclass clazz, jstring opPackageName)
  {
      ScopedUtfChars opPackageNameUtf(env, opPackageName);
      return (jlong) &SensorManager::getInstanceForPackage(String16(opPackageNameUtf.c_str()));
  }
  ```


## Magnetometer

* Magnetometer error
  ```
  08-22 01:53:11.707  2360  2360 D SensorHAL: SetDelay
  08-22 01:53:11.708  2360  2360 D SensorHAL: WriteBufferLenght: buf_len: 0.
  08-22 01:53:11.708  2360  2360 E SensorHAL: LSM9DS1 Magnetometer Sensor: Failed to write hw fifo watermark.
  08-22 01:53:11.708  2465  2796 E SensorService: sensor batch failed 0xe16d8add6c40 0x00000002 10000000 0 err=Function not implemented
  08-22 01:53:11.708  2360  2360 D SensorHAL: "LSM9DS1 Accelerometer Sensor": power-on (sensor type: 1).
  08-22 01:53:11.709  2465  2796 E SensorService: 	 >>>ERROR: activate called without batch
  ```
* frameworks/native/services/sensorservice/SensorDevice.cpp
  * status_t SensorDevice::batch()


## logcat
  
* 查看当前Log：
  ```
  01-01 00:00:13.249  2269  2269 W hwservicemanager: getTransport: Cannot find entry android.hardware.sensors@1.0::ISensors/default in either framework or device manifest.
  01-01 00:00:13.249  2367  2367 I android.hardware.memtrack@1.0-service: Registration complete for android.hardware.memtrack@1.0::IMemtrack/default.
  01-01 00:00:13.253  2363  2363 I ServiceManagement: Removing namespace from process name android.hardware.graphics.allocator@2.0-service to allocator@2.0-service.
  01-01 00:00:13.255  2363  2363 I android.hardware.graphics.allocator@2.0-service: Registration complete for android.hardware.graphics.allocator@2.0::IAllocator/default.
  01-01 00:00:13.256  2356  2356 I ServiceManagement: Removing namespace from process name android.hardware.gatekeeper@1.0-service to gatekeeper@1.0-service.
  01-01 00:00:13.264  2356  2356 I android.hardware.gatekeeper@1.0-service: Registration complete for android.hardware.gatekeeper@1.0::IGatekeeper/default.
  01-01 00:00:13.274  2380  2380 I installd: installd firing up
  01-01 00:00:13.280  2274  2274 D libEGL  : loaded /vendor/lib64/egl/libEGL_VIVANTE.so
  01-01 00:00:13.287  2269  2269 W hwservicemanager: getTransport: Cannot find entry android.hardware.thermal@1.1::IThermal/default in either framework or device manifest.
  01-01 00:00:13.290  2369  2369 D SensorHAL: 3 IIO devices available into /sys/bus/iio/devices/ folder.
  01-01 00:00:13.291  2369  2369 D SensorHAL: "lsm9ds1_gyro": IIO device found and supported. Wake-up sensor: no
  01-01 00:00:13.291  2396  2396 I /system/bin/tombstoned: tombstoned successfully initialized
  01-01 00:00:13.292  2375  2375 W thermalserviced: Unable to get Thermal HAL V1.1, vendor thermal event notification not available
  01-01 00:00:13.292  2369  2369 D SensorHAL: "lsm9ds1_magn": IIO device found and supported. Wake-up sensor: no
  01-01 00:00:13.294  2355  2355 D android.hardware.drm@1.0-service: android.hardware.drm@1.0-service starting...
  01-01 00:00:13.295  2369  2369 D SensorHAL: "lsm9ds1_accel": IIO device found and supported. Wake-up sensor: no
  01-01 00:00:13.297  2369  2369 E SensorHAL: LSM9DS1 Gyroscope Sensor: Failed to open iio char device (/dev/iio:device1).
  01-01 00:00:13.297  2369  2369 E SensorHAL: "lsm9ds1_gyro": failed to create HW sensor class.
  01-01 00:00:13.297  2369  2369 E SensorHAL: LSM9DS1 Magnetometer Sensor: Failed to open iio char device (/dev/iio:device2).
  01-01 00:00:13.297  2369  2369 E SensorHAL: "lsm9ds1_magn": failed to create HW sensor class.
  01-01 00:00:13.297  2369  2369 E SensorHAL: LSM9DS1 Accelerometer Sensor: Failed to open iio char device (/dev/iio:device0).
  01-01 00:00:13.297  2369  2369 E SensorHAL: "lsm9ds1_accel": failed to create HW sensor class.
  01-01 00:00:13.297  2369  2369 D SensorHAL: 0 sensors available and ready.
  01-01 00:00:13.298  2369  2369 I ServiceManagement: Removing namespace from process name android.hardware.sensors@1.0-service to sensors@1.0-service.
  01-01 00:00:13.299  2369  2369 I android.hardware.sensors@1.0-service: Registration complete for android.hardware.sensors@1.0::ISensors/default.
  ```
* 关闭SELinux杀死android.hardware.sensors@1.0-service进程，会重新加载HAL
  ```
  08-16 03:46:36.529  4617  4617 D SensorHAL: st_hal_open_sensors
  08-16 03:46:36.530  4617  4617 D SensorHAL: st_hal_load_iio_devices_data.
  08-16 03:46:36.530  4617  4617 D SensorHAL: 3 IIO devices available into /sys/bus/iio/devices/ folder.
  08-16 03:46:36.530  4617  4617 D SensorHAL: "lsm9ds1_gyro": IIO device found and supported. Wake-up sensor: no
  08-16 03:46:36.531  4617  4617 D SensorHAL: "lsm9ds1_magn": IIO device found and supported. Wake-up sensor: no
  08-16 03:46:36.531  4617  4617 D SensorHAL: "lsm9ds1_accel": IIO device found and supported. Wake-up sensor: no
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Gyroscope Sensor": injection mode not available.
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Gyroscope Sensor": created HW class instance, handle: 1 (sensor type: 4).
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Magnetometer Sensor": injection mode not available.
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Magnetometer Sensor": created HW class instance, handle: 2 (sensor type: 2).
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Accelerometer Sensor": injection mode not available.
  08-16 03:46:36.533  4617  4617 D SensorHAL: "LSM9DS1 Accelerometer Sensor": created HW class instance, handle: 3 (sensor type: 1).
  08-16 03:46:36.533  4617  4617 D SensorHAL: 3 sensors available and ready.
  ```


## iio tools

* lsiio -v
* iio_generic_buffer -N 1 -a