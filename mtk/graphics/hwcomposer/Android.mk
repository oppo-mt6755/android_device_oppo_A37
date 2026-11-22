LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := hwcomposer.mt6755
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := framebuffer_hwc.cpp
LOCAL_SHARED_LIBRARIES := libhardware liblog libsync libdl
LOCAL_C_INCLUDES := $(TARGET_KERNEL_SOURCE)/drivers/misc/mediatek/video/include
LOCAL_CFLAGS := -Wall -Wextra -Werror
include $(BUILD_SHARED_LIBRARY)
