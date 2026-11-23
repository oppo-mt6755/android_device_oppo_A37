/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Unsupported video-call snapshots must fail without using the old Skia ABI. */

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <log/log.h>

void _Z8SkDebugfPKcz(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_VERBOSE, "VT", format, args);
    va_end(args);
}

/* SkImageDecoder::DecodeFile(const char*, SkBitmap*, SkColorType, Mode, Format*) */
bool _ZN14SkImageDecoder10DecodeFileEPKcP8SkBitmap11SkColorTypeNS_4ModeEPNS_6FormatE(
        const char *file, void *bitmap, int colorType, int mode, void *format)
{
    (void)file; (void)bitmap; (void)colorType; (void)mode; (void)format;
    return false;
}

/* SkImageEncoder::EncodeFile(const char*, const SkBitmap&, Type, int) */
bool _ZN14SkImageEncoder10EncodeFileEPKcRK8SkBitmapNS_4TypeEi(
        const char *file, const void *bitmap, int type, int quality)
{
    (void)file; (void)bitmap; (void)type; (void)quality;
    return false;
}

/* SkImageEncoder::EncodeStream(SkWStream*, const SkBitmap&, Type, int) */
bool _ZN14SkImageEncoder12EncodeStreamEP9SkWStreamRK8SkBitmapNS_4TypeEi(
        void *stream, const void *bitmap, int type, int quality)
{
    (void)stream; (void)bitmap; (void)type; (void)quality;
    return false;
}

/* SkMemoryWStream::SkMemoryWStream(void*, size_t); only handed to EncodeStream */
void *_ZN15SkMemoryWStreamC1EPvj(void *thiz, void *buffer, size_t size)
{
    (void)buffer; (void)size;
    return thiz;
}

/* SkBitmap::tryAllocPixels(Allocator*, SkColorTable*) */
bool _ZN8SkBitmap14tryAllocPixelsEPNS_9AllocatorEP12SkColorTable(
        void *thiz, void *allocator, void *ctable)
{
    (void)thiz; (void)allocator; (void)ctable;
    return false;
}

bool _ZN8SkBitmap7setInfoERK11SkImageInfoj(void *thiz, const void *info, unsigned rowBytes)
{
    (void)thiz; (void)info; (void)rowBytes;
    return false;
}

/* No Skia object is constructed: all allocation/decoding entry points fail. */
#define EMPTY_OBJECT(symbol) void symbol(void *thiz) { (void)thiz; }
EMPTY_OBJECT(_ZN7SkPaintC1Ev)
EMPTY_OBJECT(_ZN7SkPaintD1Ev)
EMPTY_OBJECT(_ZN8SkBitmapC1Ev)
EMPTY_OBJECT(_ZN8SkBitmapD1Ev)
EMPTY_OBJECT(_ZN8SkCanvasD1Ev)
EMPTY_OBJECT(_ZN9SkWStreamD2Ev)

void _ZN8SkCanvasC1ERK8SkBitmap(void *thiz, const void *bitmap)
{
    (void)thiz; (void)bitmap;
}

void _ZN8SkCanvas10drawBitmapERK8SkBitmapffPK7SkPaint(
        void *thiz, const void *bitmap, float x, float y, const void *paint)
{
    (void)thiz; (void)bitmap; (void)x; (void)y; (void)paint;
}

void _ZN8SkCanvas5scaleEff(void *thiz, float x, float y)
{
    (void)thiz; (void)x; (void)y;
}

void _ZN8SkCanvas6concatERK8SkMatrix(void *thiz, const void *matrix)
{
    (void)thiz; (void)matrix;
}

void _ZN8SkMatrix8setScaleEffff(void *thiz, float x, float y, float px, float py)
{
    (void)thiz; (void)x; (void)y; (void)px; (void)py;
}

void _ZNK8SkBitmap10eraseColorEj(const void *thiz, unsigned color)
{
    (void)thiz; (void)color;
}
