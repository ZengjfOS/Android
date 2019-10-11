# HAL hello-world

## 参考文档

* [Android HIDL学习（2） ---- HelloWorld](https://www.jianshu.com/p/ca6823b897b5)

## 基本操作

* `source build/envsetup.sh`
* `lunch mek_8q_car2-userdebug`
* `PACKAGE=android.hardware.zengjf@1.0`
* `LOC=hardware/interfaces/zengjf/1.0/default/`
* `hidl-gen -o $LOC -Lc++-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE`
* `hidl-gen -o $LOC -Landroidbp-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE`
* `hidl-gen -Landroidbp -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE`
* `./hardware/interfaces/update-makefiles.sh`
* `touch hardware/interfaces/zengjf/1.0/default/android.hardware.zengjf@1.0-service.rc`
* `touch hardware/interfaces/zengjf/1.0/default/service.cpp`
* `tree hardware/interfaces/zengjf/1.0/` 
  ```
  hardware/interfaces/zengjf/1.0/
  ├── Android.bp
  ├── default
  │   ├── Android.bp
  │   ├── android.hardware.zengjf@1.0-service.rc
  │   ├── service.cpp
  │   ├── Zengjf.cpp
  │   └── Zengjf.h
  └── IZengjf.hal
  
  ```
* `mmm hardware/interfaces/zengjf/1.0/default`
* `mmm hardware/interfaces/zengjf/1.0/vts`
* `cat device/fsl/imx8q/mek_8q/manifest_car.xml`
  ```
  <hal format="hidl">
      <name>android.hardware.naruto</name>
      <transport>hwbinder</transport>
      <version>1.0</version>
      <interface>
          <name>INaruto</name>
          <instance>default</instance>
      </interface>
  </hal>
  ```
* 给这个进程配sepolicy，所以正确的做法是要给他加上selinux的权限，不过也可以用root权限去手动起这个service。
  * SELinux写法参考：[Writing selinuxswitch SELinux Policy](docs/0002_Writing_selinuxswitch_SELinux_Policy.md)
* `/verdor/bin/hw/android.hardware.zengjf@1.0-service`
* `zengjf_test`

## 参考代码

https://github.com/ZengjfOS/Android/tree/hal-zengjf