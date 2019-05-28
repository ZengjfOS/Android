# external/i2c-tools/Android.mk

LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES:=bmp2drm.c
LOCAL_MODULE:=bmp2drm
# LOCAL_C_INCLUDES += vendor/nxp-opensource/libdrm-imx/
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LIBDRM_IMX)/libdrm-imx/ $(LIBDRM_IMX)/libdrm-imx/include/drm/
LOCAL_SHARED_LIBRARIES := libutils libc
# LOCAL_SHARED_LIBRARIES += libdrm
LOCAL_STATIC_LIBRARIES := libdrm
LOCAL_CFLAGS += -DBUILD_FOR_ANDROID
LOCAL_CFLAGS += -DANDROID
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-sign-compare -Wno-pointer-arith -Wno-unused-variable -Wno-unused-function
# LOCAL_VENDOR_MODULE := true
include $(BUILD_EXECUTABLE)

