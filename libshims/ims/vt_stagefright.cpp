/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <log/log.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/NuMediaExtractor.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/avc_utils.h>

using android::MetaData;

extern "C" bool _ZN7android8MetaData8setInt32Eji(MetaData *self, uint32_t key, int32_t value) {
    return self->setInt32(key, value);
}

extern "C" bool _ZN7android8MetaData8setInt64Ejx(MetaData *self, uint32_t key, int64_t value) {
    return self->setInt64(key, value);
}

extern "C" bool _ZN7android8MetaData10setCStringEjPKc(MetaData *self, uint32_t key,
        const char *value) {
    return self->setCString(key, value);
}

extern "C" bool _ZN7android8MetaData9findInt32EjPi(const MetaData *self, uint32_t key,
        int32_t *value) {
    return self->findInt32(key, value);
}

extern "C" bool _ZN7android8MetaData9findInt64EjPx(const MetaData *self, uint32_t key,
        int64_t *value) {
    return self->findInt64(key, value);
}

extern "C" bool _ZN7android8MetaData11findCStringEjPPKc(const MetaData *self, uint32_t key,
        const char **value) {
    return self->findCString(key, value);
}

extern "C" android::status_t _ZN7android16NuMediaExtractor11selectTrackEj(
        android::NuMediaExtractor *self, size_t track) {
    return self->selectTrack(track);
}

extern "C" bool _ZN7android5IsIDRERKNS_2spINS_7ABufferEEE(
        const android::sp<android::ABuffer> &buffer) {
    return buffer != nullptr && android::IsIDR(buffer->data(), buffer->size());
}

/* The IMS control service does not instantiate the unsupported video-call audio
 * sources. Never construct a Q object inside the smaller vendor allocation. */
extern "C" void _ZN7android11AudioSourceC1E14audio_source_tRKNS_8String16Ejjjji(void *) {
    LOG_ALWAYS_FATAL("Legacy video-call audio sources are unsupported");
}

extern "C" void _ZThn4_N7android11MediaSourceD0Ev(void *) {
    LOG_ALWAYS_FATAL("Legacy video-call MediaSource layout is unsupported");
}

extern "C" void _ZThn4_N7android11MediaSourceD1Ev(void *) {
    LOG_ALWAYS_FATAL("Legacy video-call MediaSource layout is unsupported");
}
