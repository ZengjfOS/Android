# system platform key apk asign

内置APK签名

## 参考文档

* [Android 8.0系统给第三方apk进行系统签名（未使用Andriod Studio，在Android系统编译环境下签名）](https://blog.csdn.net/sinat_24946363/article/details/79934086)
* [vendor/google/apps/Android.mk](https://github.com/Nu3001/vendor/blob/master/google/apps/Android.mk)


## Key

```
zengjf@UbuntuServer:p9.0.0.0$ ls -al build/target/product/security/
total 60
drwxrwxr-x 2 zengjf zengjf 4096 Mar  1 13:56 .
drwxrwxr-x 4 zengjf zengjf 4096 Mar  1 13:56 ..
-rw-rw-r-- 1 zengjf zengjf  656 Mar  1 13:56 Android.mk
-rw-rw-r-- 1 zengjf zengjf 1216 Mar  1 13:56 media.pk8
-rw-rw-r-- 1 zengjf zengjf 1675 Mar  1 13:56 media.x509.pem
-rw-rw-r-- 1 zengjf zengjf 1216 Mar  1 13:56 platform.pk8
-rw-rw-r-- 1 zengjf zengjf 1675 Mar  1 13:56 platform.x509.pem
-rw-rw-r-- 1 zengjf zengjf 3128 Mar  1 13:56 README
-rw-rw-r-- 1 zengjf zengjf 1218 Mar  1 13:56 shared.pk8
-rw-rw-r-- 1 zengjf zengjf 1675 Mar  1 13:56 shared.x509.pem
-rw-rw-r-- 1 zengjf zengjf 1217 Mar  1 13:56 testkey.pk8
-rw-rw-r-- 1 zengjf zengjf 1675 Mar  1 13:56 testkey.x509.pem
-rw-rw-r-- 1 zengjf zengjf  524 Mar  1 13:56 verity_key
-rw-rw-r-- 1 zengjf zengjf 1219 Mar  1 13:56 verity.pk8
-rw-rw-r-- 1 zengjf zengjf 1444 Mar  1 13:56 verity.x509.pem
```

## sign tool

```
zengjf@UbuntuServer:p9.0.0.0$ ls -al prebuilts/sdk/tools/lib
total 24472
drwxrwxr-x 2 zengjf zengjf     4096 Mar  1 14:05 .
drwxrwxr-x 9 zengjf zengjf     4096 Mar  1 14:05 ..
-rw-rw-r-- 1 zengjf zengjf 21789584 Mar  1 14:05 d8.jar
-rw-rw-r-- 1 zengjf zengjf   991427 Mar  1 14:05 dx.jar
-rw-rw-r-- 1 zengjf zengjf    29617 Mar  1 14:05 shrinkedAndroid.jar
-rw-rw-r-- 1 zengjf zengjf  2228854 Mar  1 14:05 signapk.jar
```

## Android.mk

* https://github.com/Nu3001/vendor/blob/master/google/apps/Android.mk
* /home/zengjf/imx8src/imx8qm_android/p9.0.0.0/vendor/apps/Android.mk
  ```
  LOCAL_PATH := $(call my-dir)
  
  include $(CLEAR_VARS)
  LOCAL_MODULE := SdkSample
  LOCAL_MODULE_TAGS := optional
  LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
  LOCAL_MODULE_CLASS := APPS
  LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
  LOCAL_PRIVILEGED_MODULE := true
  LOCAL_CERTIFICATE := platform
  include $(BUILD_PREBUILT)
  
  include $(CLEAR_VARS)
  LOCAL_MODULE := SdkService
  LOCAL_MODULE_TAGS := optional
  LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
  LOCAL_MODULE_CLASS := APPS
  LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
  LOCAL_PRIVILEGED_MODULE := true
  LOCAL_CERTIFICATE := platform
  include $(BUILD_PREBUILT)
  ```