DEVICE_PATH := device/oppo/A37

include $(DEVICE_PATH)/platform/board.mk

# Platform
TARGET_BOARD_PLATFORM := mt6755
TARGET_NO_BOOTLOADER := true

# CPU
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := cortex-a53

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := cortex-a53

TARGET_BOOTLOADER_BOARD_NAME := A37

# Kernel
TARGET_USES_64_BIT_BINDER := true
BOARD_USES_FULL_RECOVERY_IMAGE := true
BOARD_KERNEL_IMAGE_NAME := Image.gz-dtb
BOARD_KERNEL_BASE := 0x40000000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_KERNEL_CMDLINE := bootopt=64S3,32N2,64N2 androidboot.selinux=permissive
BOARD_MKBOOTIMG_ARGS := --board A37 --kernel_offset 0x00080000 --ramdisk_offset 0x05000000 --second_offset 0x00f00000 --tags_offset 0x04000000
TARGET_KERNEL_ARCH := arm64
TARGET_KERNEL_CROSS_COMPILE_PREFIX := aarch64-linux-android-
TARGET_KERNEL_SOURCE := kernel/oppo/mt6750
TARGET_KERNEL_CONFIG := oppo6750_15127_debug_defconfig

# Partitions
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 16777216
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_CACHEIMAGE_PARTITION_SIZE := 268435456
BOARD_SYSTEMIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 2147483648
BOARD_USERDATAIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_USERDATAIMAGE_PARTITION_SIZE := 13048462336
BOARD_FLASH_BLOCK_SIZE := 4096

TARGET_USERIMAGES_USE_EXT4 := true

# Include needed symbols
TARGET_INCLUDE_LIBTUBA := false

# Display
DEVICE_RESOLUTION := 720x1280
TARGET_SCREEN_WIDTH := 720
TARGET_SCREEN_HEIGHT := 1280

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(DEVICE_PATH)/bluetooth

# LightHAL
TARGET_PROVIDES_LIBLIGHT := true

# Recovery
TARGET_RECOVERY_FSTAB := $(DEVICE_PATH)/recovery/fstab.mt6755

TARGET_SYSTEM_PROP := $(DEVICE_PATH)/system.prop

# SELinux
BOARD_SEPOLICY_DIRS += device/oppo/A37/sepolicy/boot

# This kernel exposes the legacy f_adb character device.
TARGET_USES_LEGACY_ADB_INTERFACE := true
TARGET_COPY_OUT_VENDOR := system/vendor
TARGET_USES_HWC2 := true
KERNEL_TOOLCHAIN := $(abspath prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin)

# Defer donor hardware daemons until each device has been brought up.
PRODUCT_COPY_FILES := $(foreach entry,$(PRODUCT_COPY_FILES),$(if $(findstring :system/vendor/etc/init/,$(entry)),,$(entry)))
# The platform builds these DRM plugins itself.
PRODUCT_COPY_FILES := $(filter-out %:system/vendor/lib/mediadrm/libdrmclearkeyplugin.so %:system/vendor/lib64/mediadrm/libdrmclearkeyplugin.so %:system/vendor/lib/mediadrm/libmockdrmcryptoplugin.so %:system/vendor/lib64/mediadrm/libmockdrmcryptoplugin.so,$(PRODUCT_COPY_FILES))

# Add compatibility symbols only to the libraries that import them.
TARGET_LD_SHIM_LIBS += /system/vendor/lib/egl/libGLES_mali.so|libmtk_atomic_compat.so
TARGET_LD_SHIM_LIBS += /system/vendor/lib/libgpu_aux.so|libmtk_atomic_compat.so
TARGET_LD_SHIM_LIBS += /system/vendor/lib64/egl/libGLES_mali.so|libmtk_atomic_compat.so
TARGET_LD_SHIM_LIBS += /system/vendor/lib64/libgpu_aux.so|libmtk_atomic_compat.so

# The donor display structs do not match the retained kernel ABI.
PRODUCT_COPY_FILES := $(filter-out %:system/vendor/lib/hw/hwcomposer.mt6755.so %:system/vendor/lib64/hw/hwcomposer.mt6755.so,$(PRODUCT_COPY_FILES))
