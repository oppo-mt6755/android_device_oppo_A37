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
    $(LOCAL_PATH)/configs/audio/audio_policy_configuration.xml:system/vendor/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/configs/audio/a2dp_audio_policy_configuration.xml:system/vendor/etc/a2dp_audio_policy_configuration.xml \
    $(LOCAL_PATH)/rootdir/fstab.mt6755:root/fstab.mt6755 \
    $(LOCAL_PATH)/rootdir/android.hardware.audio@2.0-service.rc:system/vendor/etc/init/android.hardware.audio@2.0-service.rc \
    $(LOCAL_PATH)/rootdir/init.mt6755.rc:root/init.mt6755.rc \
    $(LOCAL_PATH)/rootdir/init.mt6755.usb.rc:root/init.mt6755.usb.rc \
    $(LOCAL_PATH)/rootdir/ueventd.mt6755.rc:root/ueventd.mt6755.rc \
    $(LOCAL_PATH)/seccomp/mediaextractor.policy:system/vendor/etc/seccomp_policy/mediaextractor.policy \
    $(LOCAL_PATH)/seccomp/mediacodec.policy:system/vendor/etc/seccomp_policy/mediacodec.policy

$(call inherit-product, device/oppo/A37/platform/common.mk)
$(call inherit-product, device/oppo/A37/platform/media.mk)

# Sony userspace remains the donor; board-specific firmware is from OPPO.
$(call inherit-product, vendor/sony/tuba/tuba-vendor.mk)

PRODUCT_PACKAGES += lights.mt6755

PRODUCT_PACKAGES += android.hardware.health@1.0-impl

# Binderized interfaces for the retained connectivity and sensor hardware.
$(call inherit-product, device/oppo/A37/platform/wifi.mk)
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service
PRODUCT_COPY_FILES += \
    device/oppo/A37/rootdir/android.hardware.bluetooth@1.0-service.rc:system/vendor/etc/init/android.hardware.bluetooth@1.0-service.rc \
    device/oppo/A37/rootdir/android.hardware.sensors@1.0-service.rc:system/vendor/etc/init/android.hardware.sensors@1.0-service.rc \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml

PRODUCT_COPY_FILES += device/oppo/A37/rootdir/init.mt6755.connectivity.rc:root/init.mt6755.connectivity.rc
PRODUCT_COPY_FILES += device/oppo/A37/rootdir/android.hardware.wifi@1.0-service.rc:system/vendor/etc/init/android.hardware.wifi@1.0-service.rc

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

PRODUCT_COPY_FILES += device/oppo/A37/rootdir/init.mt6755.radio.rc:root/init.mt6755.radio.rc

PRODUCT_PACKAGES += mtk-ril librilmtk mtkc mtklog

PRODUCT_PACKAGES += ImsService libmtk_ims_bionic libmtk_ims_ifc \
    libmtk_ims_vt_compat libmtk_ims_sockets
PRODUCT_COPY_FILES += device/oppo/A37/ims/init.mt6755.ims.rc:root/init.mt6755.ims.rc

# Legacy MTK camera provider and narrow graphics compatibility imports.
PRODUCT_PACKAGES += android.hardware.camera.provider@2.4-impl mtkui libmtk_jpeg_compat
PRODUCT_COPY_FILES += device/oppo/A37/rootdir/pq.rc:system/vendor/etc/init/pq.rc
PRODUCT_COPY_FILES += device/oppo/A37/rootdir/mediaserver.rc:system/etc/init/mediaserver.rc
PRODUCT_COPY_FILES += device/oppo/A37/configs/media_profiles.xml:system/vendor/etc/media_profiles_V1_0.xml

# Binderized GNSS adapter for the donor MTK GPS HAL and navigation daemon.
PRODUCT_PACKAGES += android.hardware.gnss@1.0-impl android.hardware.gnss@1.0-service libcurl
PRODUCT_COPY_FILES += device/oppo/A37/rootdir/gps.rc:system/vendor/etc/init/gps.rc
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml
