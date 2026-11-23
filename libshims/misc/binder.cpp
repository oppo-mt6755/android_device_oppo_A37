#include <binder/IInterface.h>

android::sp<android::IBinder> legacy_interface_as_binder(const android::IInterface*)
        __asm__("_ZN7android10IInterface8asBinderEv");

android::sp<android::IBinder> legacy_interface_as_binder(const android::IInterface* self) {
    return android::IInterface::asBinder(self);
}
