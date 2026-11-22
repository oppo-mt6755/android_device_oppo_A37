#include <stdint.h>

/* Oreo inlines this operation; the stock GPU libraries import its C symbol. */
int32_t android_atomic_acquire_load(const volatile int32_t* value) {
    return __atomic_load_n(value, __ATOMIC_ACQUIRE);
}
