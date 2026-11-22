LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_atomic_compat
LOCAL_SRC_FILES := atomic.c
LOCAL_MULTILIB := both
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

# Legacy camera imports; all objects use the current platform implementation.
include $(CLEAR_VARS)
LOCAL_MODULE := mtkui
LOCAL_SRC_FILES := ui.cpp legacy_atomic.c
LOCAL_SHARED_LIBRARIES := libui libgui libsensor mtklog
LOCAL_HEADER_LIBRARIES := libcutils_headers
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_jpeg_compat
LOCAL_SRC_FILES := jpeg.c
LOCAL_SHARED_LIBRARIES := libmtkjpeg
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
