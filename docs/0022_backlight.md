# backlight

背光控制原理

## 参考文档

[android lcd 背光控制流程](https://blog.csdn.net/hbk320/article/details/48275369)

## error

```
[ 1286.086494] vehicle_rpmsg virtio0.rpmsg-vehicle-channel.-1.1: rpmsg_send timeout!
[ 1286.086543] send message failed!
06-27 07:05:48.557  2595  2595 I CAR.POWER: send display brightness = 29
[ 1286.088031] this type is not correct!
```

## Service rpmsg调用

* packages/services/Car/service/src/com/android/car/hal/PowerHalService.java
  ```Java
  /**
   * Sets the display brightness for the vehicle.
   * @param brightness value from 0 to 100.
   */
  public void sendDisplayBrightness(int brightness) {
      if (brightness < 0) {
          brightness = 0;
      } else if (brightness > 100) {
          brightness = 100;
      }
      try {
          mHal.set(VehicleProperty.DISPLAY_BRIGHTNESS, 0).to(brightness);           <--------- 修改
          Log.i(CarLog.TAG_POWER, "send display brightness = " + brightness);       <--------- 修改
      } catch (PropertyTimeoutException e) {
          Log.e(CarLog.TAG_POWER, "cannot set DISPLAY_BRIGHTNESS", e);
      }
  }
  ```
* sendDisplayBrightness
  ```
  wugn@wugn:Car$ grep sendDisplayBrightness * -R
  service/src/com/android/car/hal/PowerHalService.java:    public void sendDisplayBrightness(int brightness) {
  service/src/com/android/car/systeminterface/DisplayInterface.java:                        mService.sendDisplayBrightness(percentBright);
  service/src/com/android/car/CarPowerManagementService.java:    public void sendDisplayBrightness(int brightness) {
  service/src/com/android/car/CarPowerManagementService.java:        mHal.sendDisplayBrightness(brightness);
  ```

## HAL 调用

* frameworks/base/services/core/jni/com_android_server_lights_LightsService.cpp
  ```CPP
  static void processReturn(
          const Return<Status> &ret,
          Type type,
          const LightState &state) {
      if (!ret.isOk()) {
          ALOGE("Failed to issue set light command.");
          LightHal::disassociate();
          return;
      }
  
      switch (static_cast<Status>(ret)) {
          case Status::SUCCESS:
              break;
          case Status::LIGHT_NOT_SUPPORTED:
              ALOGE("Light requested not available on this device. %d", type);
              break;
          case Status::BRIGHTNESS_NOT_SUPPORTED:
              ALOGE("Brightness parameter not supported on this device: %d",
                  state.brightnessMode);
              break;
          case Status::UNKNOWN:
          default:
              ALOGE("Unknown error setting light.");
      }
  }
  
  static void setLight_native(
          JNIEnv* /* env */,
          jobject /* clazz */,
          jint light,
          jint colorARGB,
          jint flashMode,
          jint onMS,
          jint offMS,
          jint brightnessMode) {
  
      if (!validate(light, flashMode, brightnessMode)) {
          return;
      }
  
      sp<ILight> hal = LightHal::associate();
  
      if (hal == nullptr) {
          return;
      }
  
      Type type = static_cast<Type>(light);
      LightState state = constructState(
          colorARGB, flashMode, onMS, offMS, brightnessMode);
  
      {
          android::base::Timer t;
          Return<Status> ret = hal->setLight(type, state);
          processReturn(ret, type, state);
          if (t.duration() > 50ms) ALOGD("Excessive delay setting light");
      }
  }
  ```
* setLight_native()--->set_light ()，对应调用的是set_light_backlight来设置背光
* vendor/nxp-opensource/imx/lights/fsl_lights.c
  ```C
  /*****************************************************************************/
  static int lights_device_open(const struct hw_module_t* module,
                                const char* name, struct hw_device_t** device)
  {
      int status = -EINVAL;
      ALOGV("lights_device_open\n");
      if (!strcmp(name, LIGHT_ID_BACKLIGHT)) {
          struct light_device_t *dev;
          char value[PROPERTY_VALUE_MAX];
          FILE *file;
  
          dev = malloc(sizeof(*dev));
  
          /* initialize our state here */
          memset(dev, 0, sizeof(*dev));
  
          /* initialize the procs */
          dev->common.tag = HARDWARE_DEVICE_TAG;
          dev->common.version = 0;
          dev->common.module = (struct hw_module_t*) module;
          dev->common.close = light_close_backlight;
  
          dev->set_light = set_light_backlight;
  
          *device = &dev->common;
  
          property_get("hw.backlight.dev", value, DEF_BACKLIGHT_DEV);
          strcpy(path, DEF_BACKLIGHT_PATH);
          strcat(path, value);
          strcpy(max_path, path);
          strcat(max_path, "/max_brightness");
          strcat(path, "/brightness");
  
  
          file = fopen(max_path, "r");
          if (!file) {
              // free(dev);                                                                            <--------- 修改
              ALOGE("cannot open backlight file %s [%s]\n", max_path,strerror(errno));
              // return status;                                                                        <--------- 修改
          } else {
              fclose(file);
          }
  
          ALOGI("max backlight file is %s\n", max_path);
          ALOGI("backlight brightness file is %s\n", path);
  
          status = 0;
      }
  
      /* todo other lights device init */
      return status;
  }
  ```