/* SPDX-License-Identifier: Apache-2.0 */
#define LOG_TAG "lights"
#include <hardware/lights.h>
#include <log/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static const char brightness_path[] = "/sys/class/leds/lcd-backlight/brightness";

static int set_backlight(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    if (!state) return -EINVAL;
    if (state->brightnessMode == BRIGHTNESS_MODE_LOW_PERSISTENCE) return -ENOSYS;
    unsigned int color = state->color;
    unsigned int level = (77 * ((color >> 16) & 255) +
                          150 * ((color >> 8) & 255) + 29 * (color & 255)) >> 8;
    char value[16];
    int length = snprintf(value, sizeof(value), "%u\n", level);
    int fd = TEMP_FAILURE_RETRY(open(brightness_path, O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        int error = errno;
        ALOGE("Cannot open backlight: %s", strerror(error));
        return -error;
    }
    ssize_t result = TEMP_FAILURE_RETRY(write(fd, value, length));
    int error = result < 0 ? errno : EIO;
    close(fd);
    if (result != length) {
        ALOGE("Cannot set backlight to %u: %s", level, strerror(error));
        return -error;
    }
    return 0;
}

/* The notification and charging indicators share one white PMIC LED. */
static pthread_mutex_t indicator_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t battery_state;
static struct light_state_t notification_state;
static struct light_state_t attention_state;

static int write_indicator(const char *node, unsigned int value)
{
    char path[96];
    char text[24];
    snprintf(path, sizeof(path), "/sys/class/leds/white/%s", node);
    int length = snprintf(text, sizeof(text), "%u\n", value);
    int fd = TEMP_FAILURE_RETRY(open(path, O_WRONLY | O_CLOEXEC));
    if (fd < 0) return -errno;
    ssize_t written = TEMP_FAILURE_RETRY(write(fd, text, length));
    int result = written == length ? 0 : -(written < 0 ? errno : EIO);
    close(fd);
    return result;
}

static int apply_indicator(void)
{
    const struct light_state_t *state = &battery_state;
    if (notification_state.color & 0x00ffffff) state = &notification_state;
    if (attention_state.color & 0x00ffffff) state = &attention_state;

    unsigned int mode = 0;
    if (state->color & 0x00ffffff) {
        // The retained PMIC driver uses 1 for autonomous breathing and
        // every value from 2 to 255 for the same steady brightness.
        mode = state->flashMode == LIGHT_FLASH_NONE ? 255 : 1;
    }
    return write_indicator("brightness", mode);
}

static int set_indicator(struct light_state_t *saved, const struct light_state_t *state)
{
    if (!state) return -EINVAL;
    pthread_mutex_lock(&indicator_lock);
    *saved = *state;
    int result = apply_indicator();
    pthread_mutex_unlock(&indicator_lock);
    return result;
}

static int set_battery(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&battery_state, state);
}

static int set_notification(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&notification_state, state);
}

static int set_attention(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    return set_indicator(&attention_state, state);
}

static int close_light(struct hw_device_t *device)
{
    free(device);
    return 0;
}

static int open_light(const struct hw_module_t *module, const char *name,
                      struct hw_device_t **result)
{
    if (!name || !result) return -EINVAL;
    int (*set_light)(struct light_device_t *, const struct light_state_t *);
    if (!strcmp(name, LIGHT_ID_BACKLIGHT)) set_light = set_backlight;
    else if (!strcmp(name, LIGHT_ID_BATTERY)) set_light = set_battery;
    else if (!strcmp(name, LIGHT_ID_NOTIFICATIONS)) set_light = set_notification;
    else if (!strcmp(name, LIGHT_ID_ATTENTION)) set_light = set_attention;
    else return -ENOSYS;
    struct light_device_t *device = calloc(1, sizeof(*device));
    if (!device) return -ENOMEM;
    device->common.tag = HARDWARE_DEVICE_TAG;
    device->common.version = LIGHTS_DEVICE_API_VERSION_2_0;
    device->common.module = (struct hw_module_t *)module;
    device->common.close = close_light;
    device->set_light = set_light;
    *result = &device->common;
    return 0;
}

static struct hw_module_methods_t methods = { .open = open_light };
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = LIGHTS_HEADER_VERSION,
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "MTK sysfs backlight",
    .author = "LineageOS",
    .methods = &methods,
};
