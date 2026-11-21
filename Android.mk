LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),A37)

include $(LOCAL_PATH)/firmware.mk
include $(call all-makefiles-under,$(LOCAL_PATH))

endif
