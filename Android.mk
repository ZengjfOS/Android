LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := mdio-tool.c
LOCAL_MODULE := mdio-tool
LOCAL_CPPFLAGS += -DANDROID
LOCAL_SHARED_LIBRARIES := libc
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/$(KERNEL_DIR)/include
include $(BUILD_EXECUTABLE)
