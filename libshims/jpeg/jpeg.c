// The factory encoder uses MTK's extended v62 structures. Forward only its
// JPEG imports to the vendor codec that implements that ABI.
#include <stddef.h>
extern void* jpeg_std_error_MTK(void*);
extern void jpeg_CreateCompress_MTK(void*, int, size_t);
extern void jpeg_set_defaults_MTK(void*);
extern void jpeg_set_quality_MTK(void*, int, int);
extern void jpeg_mem_dest_MTK(void*, unsigned char**, unsigned long*);
extern void jpeg_start_compress_MTK(void*, int);
extern unsigned jpeg_write_raw_data_MTK(void*, void*, unsigned);
extern void jpeg_finish_compress_MTK(void*);
extern void jpeg_destroy_compress_MTK(void*);
void* jpeg_std_error(void* error) { return jpeg_std_error_MTK(error); }
void jpeg_CreateCompress(void* info, int version, size_t size) {
    jpeg_CreateCompress_MTK(info, version, size);
}
void jpeg_set_defaults(void* info) { jpeg_set_defaults_MTK(info); }
void jpeg_set_quality(void* info, int quality, int baseline) {
    jpeg_set_quality_MTK(info, quality, baseline);
}
void jpeg_mem_dest(void* info, unsigned char** buffer, unsigned long* size) {
    jpeg_mem_dest_MTK(info, buffer, size);
}
void jpeg_start_compress(void* info, int tables) { jpeg_start_compress_MTK(info, tables); }
unsigned jpeg_write_raw_data(void* info, void* data, unsigned lines) {
    return jpeg_write_raw_data_MTK(info, data, lines);
}
void jpeg_finish_compress(void* info) { jpeg_finish_compress_MTK(info); }
void jpeg_destroy_compress(void* info) { jpeg_destroy_compress_MTK(info); }
