LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := zengjf_test
LOCAL_SRC_FILES := \
    main.cpp 

LOCAL_CPPFLAGS += -Wno-unused-variable

LOCAL_SHARED_LIBRARIES := \
   liblog \
   libhidlbase \
   libutils \
   android.hardware.zengjf@1.0 \

include $(BUILD_EXECUTABLE)
