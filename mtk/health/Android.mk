LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libhealthd.mt6755
LOCAL_SRC_FILES := health.cpp
LOCAL_HEADER_LIBRARIES := libhealthd_headers
LOCAL_C_INCLUDES := system/core/base/include
LOCAL_MULTILIB := both
LOCAL_CFLAGS := -Wall -Wextra -Werror
include $(BUILD_STATIC_LIBRARY)
