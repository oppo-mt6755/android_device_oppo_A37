/*
 * Legacy camera performance hints are optional. The old Binder client allocates
 * an incompatible Parcel on its stack; no MTK perfservice exists on this build.
 * Refuse registration and leave pixel processing to the normal CPU governor.
 */
#include <errno.h>

int PerfServiceNative_userRegScn(void) {
    errno = ENOSYS;
    return -1;
}

int PerfServiceNative_userRegBigLittle(int big_min, int big_max, int little_min, int little_max) {
    (void)big_min; (void)big_max; (void)little_min; (void)little_max;
    errno = ENOSYS;
    return -1;
}

void PerfServiceNative_userRegScnConfig(int handle, int command,
        int first, int second, int third, int fourth) {
    (void)handle; (void)command;
    (void)first; (void)second; (void)third; (void)fourth;
}

void PerfServiceNative_userEnable(int handle) { (void)handle; }
void PerfServiceNative_userDisable(int handle) { (void)handle; }
void PerfServiceNative_userUnreg(int handle) { (void)handle; }
void PerfServiceNative_userUnregScn(int handle) { (void)handle; }
