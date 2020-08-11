# fastboot disable-verity

fastboot使用方法

* fastboot命令是在Windows中cmd中下命令，不是在u-boot中下命令
* 目前很多ARM SoC的烧录是用fastboot进行系统烧录：
  * 将SoC内部ROM程序通过USB加载u-boot；
  * fastboot和u-boot通过USB将各分区镜像写入eMMC中；

## 参考文档

* [i.mx6q sabre-SD board android 8.0  fastboot  "device is LOCKed"](https://community.nxp.com/thread/474419)
* [Android平台OEM解锁分析](http://www.tjtech.me/analyze-oem-unlocking-under-android.html)

## 基本操作

* 首先需要打开开发者模式中的OEM unlocking开关
* cmd: `adb reboot bootloader`
* cmd: `fastboot flashing unlock`
* cmd: `fastboot getvar unlocked`

## cmd输出信息

```
C:\Users\ad>fastboot flashing unlock
OKAY [  0.011s]
Finished. Total time: 0.021s
C:\Users\ad>fastboot getvar unlocked
unlocked: yes
Finished. Total time: 0.016s
```

## i.MX8 XEN 参考

* Android_User's_Guide.pdf -- 6.2.3 DM-verity configuration
* `xl create <xen config file >`
* `fastboot oem unlock`
* `xl console DomU`