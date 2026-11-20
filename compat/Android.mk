LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_atomic_compat
LOCAL_SRC_FILES := atomic.c
LOCAL_MULTILIB := both
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

