LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_bionic
LOCAL_SRC_FILES := bionic.cpp
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_ifc
LOCAL_SRC_FILES := ifc_ccmni.c
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := ImsService
LOCAL_SRC_FILES := service/ImsService.apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := .apk
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_DEX_PREOPT := false
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_vt_compat
LOCAL_SRC_FILES := vt_media.c vt_skia.c vt_ui.cpp
LOCAL_SHARED_LIBRARIES := liblog libui libutils
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MULTILIB := 32
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_sockets
LOCAL_SRC_FILES := sockets.c
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MULTILIB := 32
include $(BUILD_SHARED_LIBRARY)
