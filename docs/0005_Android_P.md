# Android P

Android P源代码下载编译

## 硬件要求

* Ubuntu 16.04 64bit version 
* openjdk-8-jdk
* 16 GB RAM
* 300 GB hard disk


## Ubuntu版本

ubuntu-16.04.6-desktop-amd64


## Android BSP包及相关文档

* [IMXANDROID: Android OS for i.MX Applications Processors](https://www.nxp.com/design/development-boards/i.mx-evaluation-and-development-boards/android-os-for-i.mx-applications-processors:IMXANDROID?tab=Documentation_Tab)
* [BSP, Drivers and Middleware](https://www.nxp.com/design/development-boards/i.mx-evaluation-and-development-boards/android-os-for-i.mx-applications-processors:IMXANDROID?&tab=Design_Tools_Tab)


## 更换Ubuntu软件源

* 参考：
  * 清华源：https://mirror.tuna.tsinghua.edu.cn/help/ubuntu/
  * 阿里源：https://developer.aliyun.com/mirror/ubuntu
* `sudo cp /etc/apt/source.list /etc/apt/source.list.bak`
* `sudo vim /etc/apt/sources.list`
  ```
  deb http://mirrors.aliyun.com/ubuntu/ xenial main
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial main
  
  deb http://mirrors.aliyun.com/ubuntu/ xenial-updates main
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial-updates main
  
  deb http://mirrors.aliyun.com/ubuntu/ xenial universe
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial universe
  deb http://mirrors.aliyun.com/ubuntu/ xenial-updates universe
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial-updates universe
  
  deb http://mirrors.aliyun.com/ubuntu/ xenial-security main
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial-security main
  deb http://mirrors.aliyun.com/ubuntu/ xenial-security universe
  deb-src http://mirrors.aliyun.com/ubuntu/ xenial-security universe
  ```
* `sudo apt-get update`


## 安装AOSP Ubuntu依赖

```shell
#!/bin/bash

sudo apt-get update
sudo apt-get install -y uuid uuid-dev \
	zlib1g-dev liblz-dev \
	liblzo2-2 liblzo2-dev \
	lzop \
	git-core curl \
	u-boot-tools \
	mtd-utils \
	android-tools-fsutils \
	openjdk-8-jdk \
	device-tree-compiler \
	gdisk \
	liblz4-tool \
	m4 \
	libz-dev 
sudo apt-get install -y vim
```


## Install Samba

* sudo apt-get install samba samba-common-bin
* sudo smbpasswd -a zengjf
* sudo vim /etc/samba/smb.conf
  ```
  [...省略]
  #======================= Share Definitions =======================
  
  # Un-comment the following (and tweak the other settings below to suit)
  # to enable the default home directory shares. This will share each
  # user's home directory as \\server\username
  [homes]
     comment = Home Directories
     browseable = no
  
  # By default, the home directories are exported read-only. Change the
  # next parameter to 'no' if you want to be able to write to them.
     read only = no
  
  # File creation mask is set to 0700 for security reasons. If you want to
  # create files with group=rw permissions, set next parameter to 0775.
     create mask = 0700
  
  # Directory creation mask is set to 0700 for security reasons. If you want to
  # create dirs. with group=rw permissions, set next parameter to 0775.
  ;   directory mask = 0700
  
  [...省略]
  ```
* ifconfig获取虚拟网卡IP
  ```
  ens33     Link encap:Ethernet  HWaddr 00:0c:29:2b:13:45  
            inet addr:192.168.40.129  Bcast:192.168.40.255  Mask:255.255.255.0
            inet6 addr: fe80::d706:b357:b851:fae3/64 Scope:Link
            UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
            RX packets:5083460 errors:2067 dropped:10 overruns:0 frame:0
            TX packets:1856310 errors:0 dropped:0 overruns:0 carrier:0
            collisions:0 txqueuelen:1000 
            RX bytes:6645705115 (6.6 GB)  TX bytes:102631434 (102.6 MB)
            Interrupt:19 Base address:0x2000 
  
  lo        Link encap:Local Loopback  
            inet addr:127.0.0.1  Mask:255.0.0.0
            inet6 addr: ::1/128 Scope:Host
            UP LOOPBACK RUNNING  MTU:65536  Metric:1
            RX packets:10014 errors:0 dropped:0 overruns:0 frame:0
            TX packets:10014 errors:0 dropped:0 overruns:0 carrier:0
            collisions:0 txqueuelen:1000 
            RX bytes:1179157 (1.1 MB)  TX bytes:1179157 (1.1 MB)
  ```
* Win10连接Samba  
* Samba连接正常  


## 获取Android P源代码

* i.MX8 Android参考文档：[IMXANDROID: Android OS for i.MX Applications Processors](https://www.nxp.com/design/development-boards/i.mx-evaluation-and-development-boards/android-os-for-i.mx-applications-processors:IMXANDROID?tab=Documentation_Tab)
  * [how to Download imx-p9.0.0_2.0.0-ga.tar.gz from nxp.com?](https://community.nxp.com/thread/501070)
* 将下载好的imx-p9.0.0_2.3.0.tar.gz通过Samba放到Ubuntu的zengjf账号的家目录；
  ```Console
  zengjf@zengjf:~$ ls
  bin               imx-p9.0.0_2.3.0.tar.gz  Videos
  Desktop           Music                    VMwareTools-10.2.5-8068393.tar.gz
  Documents         Pictures                 vmware-tools-distrib
  Downloads         Public
  examples.desktop  Templates
  ```
* git config --global user.email "zengjf42@163.com"
* git config --global user.name ZengjfOS
* mkdir ~/zengjf/
* cp imx-p9.0.0_2.3.0.tar.gz ~/zengjf
* cd ~/zengjf 
* 获取repo执行脚本内容
  ```shell
  #!/bin/bash
  
  mkdir ~/bin
  curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
  chmod a+x ~/bin/repo
  export PATH=${PATH}:~/bin
  ```
* tar xzvf imx-p9.0.0_2.3.0.tar.gz
* 更换AOSP源
  * 参考：
    * https://mirror.tuna.tsinghua.edu.cn/help/AOSP/
    * https://mirrors.tuna.tsinghua.edu.cn/help/git-repo/
  * 修改imx-p9.0.0_2.3.0/imx_android_setup.sh文件
    ```shell
    [...省略]
    export REPO_URL='https://mirrors.tuna.tsinghua.edu.cn/git/git-repo/'                                          # 设置repo默认使用路径
    if [ ! -d "$android_builddir" ]; then
        # Create android build dir if it does not exist.
        mkdir "$android_builddir"
        cd "$android_builddir"
        repo init -u https://source.codeaurora.org/external/imx/imx-manifest.git -b imx-android-pie -m imx-p9.0.0_2.3.0.xml
          rc=$?
          if [ "$rc" != 0 ]; then
             echo "---------------------------------------------------"
             echo "-----Repo Init failure"
             echo "---------------------------------------------------"
             return 1
          fi
          sed -i -e "s/android.googlesource.com/aosp.tuna.tsinghua.edu.cn/" $android_builddir/.repo/manifests/*   # 改写android源目标路径
    else
        cd "$android_builddir"
    fi
    [...省略]
    ```
* source imx-p9.0.0_2.3.0/imx_android_setup.sh
  * **注意**：如果出现
* export MY_ANDROID=~/zengjf/android_build
* source build/envsetup.sh
  ```
  including device/fsl/imx6dq/vendorsetup.sh
  including device/fsl/imx6sl/vendorsetup.sh
  including device/fsl/imx6sx/vendorsetup.sh
  including device/fsl/imx7d/vendorsetup.sh
  including device/fsl/imx7ulp/vendorsetup.sh
  including device/fsl/imx8m/vendorsetup.sh
  including device/fsl/imx8q/vendorsetup.sh
  including device/generic/car/vendorsetup.sh
  including device/generic/mini-emulator-arm64/vendorsetup.sh
  including device/generic/mini-emulator-armv7-a-neon/vendorsetup.sh
  including device/generic/mini-emulator-mips64/vendorsetup.sh
  including device/generic/mini-emulator-mips/vendorsetup.sh
  including device/generic/mini-emulator-x86_64/vendorsetup.sh
  including device/generic/mini-emulator-x86/vendorsetup.sh
  including device/generic/uml/vendorsetup.sh
  including device/google/crosshatch/vendorsetup.sh
  including device/google/cuttlefish/vendorsetup.sh
  including device/google/marlin/vendorsetup.sh
  including device/google/muskie/vendorsetup.sh
  including device/google/taimen/vendorsetup.sh
  including device/linaro/hikey/vendorsetup.sh
  including sdk/bash_completion/adb.bash
  ```
* lunch
  ```
  You're building on Linux
  
  Lunch menu... pick a combo:
       [...省略]
       23. evk_8mq-user
       24. evk_8mq-userdebug
       25. evk_8mq_drm-user
       26. evk_8mq_drm-userdebug
       27. evk_8mm-user
       28. evk_8mm-userdebug
       29. evk_8mm_drm-user
       30. evk_8mm_drm-userdebug
       31. aiy_8mq-user
       32. aiy_8mq-userdebug
       33. evk_8mn-user
       34. evk_8mn-userdebug
       35. mek_8q-user
       36. mek_8q-userdebug
       37. mek_8q_car-user
       38. mek_8q_car-userdebug
       39. mek_8q_car2-user
       40. mek_8q_car2-userdebug
       [...省略]
  ```
* lunch mek_8q_car2-userdebug
  ```
  ============================================
  PLATFORM_VERSION_CODENAME=REL
  PLATFORM_VERSION=9
  TARGET_PRODUCT=mek_8q_car2
  TARGET_BUILD_VARIANT=userdebug
  TARGET_BUILD_TYPE=release
  TARGET_ARCH=arm64
  TARGET_ARCH_VARIANT=armv8-a
  TARGET_CPU_VARIANT=cortex-a53
  TARGET_2ND_ARCH=arm
  TARGET_2ND_ARCH_VARIANT=armv7-a-neon
  TARGET_2ND_CPU_VARIANT=cortex-a9
  HOST_ARCH=x86_64
  HOST_2ND_ARCH=x86
  HOST_OS=linux
  HOST_OS_EXTRA=Linux-4.15.0-45-generic-x86_64-Ubuntu-16.04.6-LTS
  HOST_CROSS_OS=windows
  HOST_CROSS_ARCH=x86
  HOST_CROSS_2ND_ARCH=x86_64
  HOST_BUILD_TYPE=release
  BUILD_ID=2.1.0-auto-ga-rc4
  OUT_DIR=out
  ============================================
  ```
* make 2>&1 | tee build-log.txt
  ```
  ============================================
  PLATFORM_VERSION_CODENAME=REL
  PLATFORM_VERSION=9
  TARGET_PRODUCT=mek_8q_car2
  TARGET_BUILD_VARIANT=userdebug
  TARGET_BUILD_TYPE=release
  TARGET_ARCH=arm64
  TARGET_ARCH_VARIANT=armv8-a
  TARGET_CPU_VARIANT=cortex-a53
  TARGET_2ND_ARCH=arm
  TARGET_2ND_ARCH_VARIANT=armv7-a-neon
  TARGET_2ND_CPU_VARIANT=cortex-a9
  HOST_ARCH=x86_64
  HOST_2ND_ARCH=x86
  HOST_OS=linux
  HOST_OS_EXTRA=Linux-4.15.0-45-generic-x86_64-Ubuntu-16.04.6-LTS
  HOST_CROSS_OS=windows
  HOST_CROSS_ARCH=x86
  HOST_CROSS_2ND_ARCH=x86_64
  HOST_BUILD_TYPE=release
  BUILD_ID=2.1.0-auto-ga-rc4
  OUT_DIR=out
  ============================================
  ninja: no work to do.
  [1/1] out/soong/.bootstrap/bin/soong_build out/soong/build.ninja
  out/build-mek_8q_car2-cleanspec.ninja is missing, regenerating...
  out/build-mek_8q_car2.ninja is missing, regenerating...
  [1/1005] including out/soong/Android-mek_8q_car2.mk ...
  [2/1005] including art/Android.mk ...
  [3/1005] including bionic/Android.mk ...
  [4/1005] including bootable/recovery/Android.mk ...
  [5/1005] including build/make/Android.mk ...
  device/fsl/imx8q/mek_8q/AndroidUboot.mk:21: error: please install arm-none-eabi-gcc toolchain and set the installed path to ARMGCC_DIR.
  07:32:44 ckati failed with: exit status 1
  ```
* `ARMGCC_DIR`是编译单片机程序的编译器，Car版本需要单独配置，在User Guild中有描述，貌似新版本的没有，老一点版本的有；

