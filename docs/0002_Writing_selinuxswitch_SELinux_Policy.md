# Writing selinuxswitch SELinux Policy

## 参考文档

* [Customizing SELinux](https://source.android.com/security/selinux/customize)
* [Writing SELinux Policy](https://source.android.com/security/selinux/device-policy)

## 文件位置

* `device/fsl/imx8q/etc/selinuxswitch`
  ```Shell
  #!/bin/sh
  echo "change the role permission"
  setenforce 0
  #setenforce 1
  ```

## 系统拷贝

* `device/fsl/imx8q/mek_8q/BoardConfig.mk`
  ```
  PRODUCT_COPY_FILES += \
          device/fsl/imx8q/etc/selinuxswitch:vendor/bin/selinuxswitch
  ```

## 启动运行

* `device/fsl/imx8q/mek_8q/init_car.rc`
  ```
  [...省略]
  on property:vendor.all.setup_core.ready=1
      start boot_completed_main_sh
      start selinuxswitch
  [...省略]
  service selinuxswitch /vendor/bin/selinuxswitch
     class main
     user root
     group root shell
     disabled
     oneshot
  [...省略]
  ```

## SELinux

* device/fsl/imx8q/sepolicy_car/selinuxswitch.te
  ```
  type selinuxswitch, domain;                                                                       # selinuxswitch继承domain的安全策略
  permissive selinuxswitch;                                                                         # 标记为permissive domain
  type selinuxswitch_exec, exec_type, vendor_file_type, file_type;                                  # Android中的一个命名习惯，一般domain加上后缀_exec就是关联的type
  
  init_daemon_domain(selinuxswitch)                                                                 # init_daemon_domain是一个宏，用来使selinuxswitch域生效
  
  set_prop(selinuxswitch, public_vendor_default_prop)
  allow selinuxswitch shell_exec:file { map read execute getattr setattr create append };
  ```
* device/fsl/imx8q/sepolicy_car/file_contexts
  ```
  /vendor/bin/evs_service                 u:object_r:evs_service_exec:s0
  /vendor/bin/selinuxswitch               u:object_r:selinuxswitch_exec:s0                          # 设置selinuxswitch文件对应selinuxswitch_exec
  
  # Persist STSensors calibration filesystem
  /persist/STCalibration(/.*)?            u:object_r:persist_STSensors_file:s0
  
  # STSensors_deamon
  /system/bin/STSensors_daemon            u:object_r:STSensors_exec:s0
  ```


## 修改SELinux报错

* Error：
  ```
  [ 530.748281] init: Service 'vendor.sensors-hal-1-0' (pid 3731) received signal 15 
  [ 530.755847] init: Sending signal 9 to service 'vendor.sensors-hal-1-0' (pid 3731) process group... 
  [ 530.765260] libprocessgroup: Successfully killed process cgroup uid 1000 pid 3731 in 0ms 
  [ 530.774552] init: starting service 'vendor.sensors-hal-1-0'... 
  [ 530.820472] type=1400 audit(156635 1060.028:2718): avc: denied { call } for pid=2253 comm="surfaceflinger" scontext=u:r:surfaceflinger:s0 tcontext=u:r:mediacodec:s0 tclass=binder permissive=1 b/77924251 
  [ 530.838455] type=1400 audit(1566351086.288:2719): avc: denied { read } for pid=4549 comm="android.hardwar" name="iio:device1" dev="tmpfs" ino=16734 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1 
  [ 530.860517] type=1400 audit(1566351086.288:2719): avc: denied { read } for pid=4549 comm="android.hardwar" name="iio:device1" dev="tmpfs" ino=16734 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1 
  [ 530.882399] type=1400 audit(1566351086.288:2720): avc: denied { open } for pid=4549 comm="android.hardwar" path="/dev/iio:device1" dev="tmpfs" ino=16734 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1 
  [ 530.904925] type=1400 audit(1566351086.288:2720): avc: denied { open } for pid=4549 comm="android.hardwar" path="/dev/iio:device1" dev="tmpfs" ino=16734 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1 
  [ 530.927244] type=1400 audit(1566351086.288:2721): avc: denied { ioctl } for pid=4549 comm="android.hardwar" path="/dev/iio:device1" dev="tmpfs" ino=16734 ioctlcmd=0x6990 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1 
  ```
* `[ 530.927244] type=1400 audit(1566351086.288:2721): avc: denied { ioctl } for pid=4549 comm="android.hardwar" path="/dev/iio:device1" dev="tmpfs" ino=16734 ioctlcmd=0x6990 scontext=u:r:hal_sensors_default:s0 tcontext=u:object_r:iio_device:s0 tclass=chr_file permissive=1`
  * { ioctl } 
  * hal_sensors_default
  * iio_device
  * chr_file
* device/fsl/imx8q/sepolicy/hal_sensors_default.te
  ```
  allow hal_sensors_default input_device:dir { read } ;
  allow hal_sensors_default sysfs:dir { read open } ;
  allow hal_sensors_default input_device:dir { open search }  ;
  allow hal_sensors_default input_device:chr_file { ioctl read open }  ;
  allow hal_sensors_default sysfs:chr_file { open }  ;
  allow hal_sensors_default sysfs:file { open read write getattr ioctl };
  allow hal_sensors_default device:dir { open read };
  allow hal_sensors_default sensors_device:chr_file { open read };
  allow hal_sensors_default iio_device:chr_file { ioctl read open };            # 添加
  ```