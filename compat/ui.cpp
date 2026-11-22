// Legacy GraphicBuffer constructors, following the Hermes mtkui shim.
#include <new>
#include <ui/GraphicBuffer.h>

extern "C" void legacy_graphic_buffer(android::GraphicBuffer*, uint32_t,
        uint32_t, int, uint32_t) __asm__("_ZN7android13GraphicBufferC1Ejjij");
extern "C" void legacy_graphic_buffer(android::GraphicBuffer* self,
        uint32_t width, uint32_t height, int format, uint32_t usage) {
    new (self) android::GraphicBuffer(width, height, format, usage, "<Unknown>");
}

extern "C" void legacy_graphic_buffer_handle(android::GraphicBuffer*, uint32_t,
        uint32_t, int, uint32_t, uint32_t, native_handle_t*, bool)
        __asm__("_ZN7android13GraphicBufferC1EjjijjP13native_handleb");
extern "C" void legacy_graphic_buffer_handle(android::GraphicBuffer* self,
        uint32_t width, uint32_t height, int format, uint32_t usage,
        uint32_t stride, native_handle_t* handle, bool own) {
    new (self) android::GraphicBuffer(width, height, format, 1, usage, stride, handle, own);
}

// Hermes libshim_mmsdk compatibility for the effect HAL's queue imports.
#include <gui/BufferQueue.h>
#include <gui/BufferItemConsumer.h>

extern "C" void legacy_buffer_queue(
        android::sp<android::IGraphicBufferProducer>*,
        android::sp<android::IGraphicBufferConsumer>*, const void*)
        __asm__("_ZN7android11BufferQueue17createBufferQueueEPNS_2spINS_22IGraphicBufferProducerEEEPNS1_INS_22IGraphicBufferConsumerEEERKNS1_INS_19IGraphicBufferAllocEEE");
extern "C" void legacy_buffer_queue(
        android::sp<android::IGraphicBufferProducer>* producer,
        android::sp<android::IGraphicBufferConsumer>* consumer, const void*) {
    android::BufferQueue::createBufferQueue(producer, consumer, false);
}

extern "C" void legacy_buffer_from_window(android::GraphicBuffer*,
        ANativeWindowBuffer*, bool)
        __asm__("_ZN7android13GraphicBufferC1EP19ANativeWindowBufferb");
extern "C" void legacy_buffer_from_window(android::GraphicBuffer* self,
        ANativeWindowBuffer* buffer, bool own) {
    new (self) android::GraphicBuffer(buffer->handle,
            own ? android::GraphicBuffer::TAKE_HANDLE : android::GraphicBuffer::WRAP_HANDLE,
            buffer->width, buffer->height, buffer->format, 1,
            static_cast<uint64_t>(static_cast<uint32_t>(buffer->usage_deprecated)), buffer->stride);
}

extern "C" void legacy_buffer_item_consumer(android::BufferItemConsumer*,
        const android::sp<android::IGraphicBufferConsumer>&, uint32_t, int, bool)
        __asm__("_ZN7android18BufferItemConsumerC1ERKNS_2spINS_22IGraphicBufferConsumerEEEjib");
extern "C" void legacy_buffer_item_consumer(android::BufferItemConsumer* self,
        const android::sp<android::IGraphicBufferConsumer>& consumer,
        uint32_t usage, int count, bool controlled) {
    new (self) android::BufferItemConsumer(consumer, static_cast<uint64_t>(usage), count, controlled);
}

#include <ui/GraphicBufferMapper.h>
extern "C" int legacy_mapper_lock(android::GraphicBufferMapper*, buffer_handle_t,
        int, const android::Rect&, void**)
        __asm__("_ZN7android19GraphicBufferMapper4lockEPK13native_handleiRKNS_4RectEPPv");
extern "C" int legacy_mapper_lock(android::GraphicBufferMapper* self,
        buffer_handle_t handle, int usage, const android::Rect& bounds, void** data) {
    return self->lock(handle, static_cast<uint32_t>(usage), bounds, data);
}
