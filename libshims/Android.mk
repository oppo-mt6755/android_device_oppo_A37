LOCAL_PATH := $(call my-dir)

# Miscellaneous legacy C imports, following the Hermes libshims layout.
include $(CLEAR_VARS)
LOCAL_MODULE := mtkc
LOCAL_SRC_FILES := misc/libc.cpp
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := mtklog
LOCAL_SRC_FILES := misc/liblog.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_atomic
LOCAL_SRC_FILES := misc/atomic.c
LOCAL_MULTILIB := both
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

# Camera imports forward to the current platform graphics implementation.
include $(CLEAR_VARS)
LOCAL_MODULE := mtkui
LOCAL_SRC_FILES := ui/libui.cpp ui/atomic.c
LOCAL_SHARED_LIBRARIES := libui libgui libsensor mtklog
LOCAL_HEADER_LIBRARIES := libcutils_headers
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_jpeg
LOCAL_SRC_FILES := jpeg/jpeg.c
LOCAL_SHARED_LIBRARIES := libmtkjpeg
LOCAL_MULTILIB := 32
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

# Native IMS imports remain scoped to their individual vendor consumers.
include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_bionic
LOCAL_SRC_FILES := ims/bionic.cpp
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_ifc
LOCAL_SRC_FILES := ims/ifc_ccmni.c
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libshim_ims_vt
LOCAL_SRC_FILES := ims/vt_media.c ims/vt_skia.c ims/vt_ui.cpp
LOCAL_SHARED_LIBRARIES := liblog libui libutils
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MULTILIB := 32
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libmtk_ims_sockets
LOCAL_SRC_FILES := ims/sockets.c
LOCAL_SHARED_LIBRARIES := libc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MULTILIB := 32
include $(BUILD_SHARED_LIBRARY)
