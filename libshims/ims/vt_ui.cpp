/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <new>
#include <ui/GraphicBuffer.h>

extern "C" void* _ZN7android13GraphicBufferC1EP19ANativeWindowBufferb(
        void* self, ANativeWindowBuffer* buffer, bool keepOwnership) {
    return new (self) android::GraphicBuffer(buffer->width, buffer->height,
            buffer->format, 1, static_cast<uint32_t>(buffer->usage_deprecated),
            buffer->stride, const_cast<native_handle_t*>(buffer->handle), keepOwnership);
}

extern "C" android::status_t _ZN7android13GraphicBuffer4lockEjPPv(
        android::GraphicBuffer *self, uint32_t usage, void **address) {
    return self->lock(usage, address);
}
