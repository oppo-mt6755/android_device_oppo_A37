LOCAL_PATH := device/oppo/A37

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi
PRODUCT_AAPT_PREBUILT_DPI := xhdpi hdpi
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    $(LOCAL_PATH)/configs/manifest.xml:system/vendor/manifest.xml \
    $(LOCAL_PATH)/configs/audio/boot_audio_policy_configuration.xml:system/vendor/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/configs/audio/a2dp_audio_policy_configuration.xml:system/vendor/etc/a2dp_audio_policy_configuration.xml \
    $(LOCAL_PATH)/rootdir/fstab.mt6755:root/fstab.mt6755 \
    $(LOCAL_PATH)/rootdir/init.mt6755.rc:root/init.mt6755.rc \
    $(LOCAL_PATH)/rootdir/init.mt6755.usb.rc:root/init.mt6755.usb.rc \
    $(LOCAL_PATH)/rootdir/ueventd.mt6755.rc:root/ueventd.mt6755.rc \
    $(LOCAL_PATH)/seccomp/mediaextractor.policy:system/vendor/etc/seccomp_policy/mediaextractor.policy \
    $(LOCAL_PATH)/seccomp/mediacodec.policy:system/vendor/etc/seccomp_policy/mediacodec.policy

$(call inherit-product, device/oppo/A37/platform/common.mk)
$(call inherit-product, device/oppo/A37/platform/media.mk)

# Sony userspace remains the donor; board-specific firmware is from OPPO.
$(call inherit-product, vendor/sony/tuba/tuba-vendor.mk)
