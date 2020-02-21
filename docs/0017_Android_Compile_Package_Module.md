# Android P Add APK

## 参考文档

* [Android P系统编译打包时去掉不必要的apk](https://blog.csdn.net/Sunxiaolin2016/article/details/91612992)


## 示例

* PRODUCT_PACKAGES
  * build/make/target/product/core.mk
  * build/make/target/product/generic_no_telephony.mk
  * build/make/target/product/core_minimal.mk
  * build/make/target/product/sdk_base.mk
  * packages/services/Car/car_product/build/car.mk
  * device/fsl/imx8q/ProductConfigCommon.mk
* device/fsl/imx8q/ProductConfigCommon.mk
  ```
  # Android infrastructures
  PRODUCT_PACKAGES += \
      CactusPlayer \
      FSLOta \
      charger_res_images \
      libGLES_android \
      libRS \
      libedid \
      libion \
      librs_jni \
      verity_warning_images \
      vndk-sp \
      CarRadio \
      CarHvac \
      UsbSwitch \
      SdkService
  ```