package com.mediatek.internal.telephony;

import android.os.IBinder;
import android.os.RemoteException;

/* Adapt the vendor's capability query to the selected modem slot on AOSP. */
public interface ITelephonyEx {
    int getMainCapabilityPhoneId() throws RemoteException;
    void setTrmForPhone(int phoneId, int mode) throws RemoteException;

    abstract class Stub {
        private static final ITelephonyEx COMPAT = new ITelephonyEx() {
            @Override public int getMainCapabilityPhoneId() {
                try {
                    Class<?> properties = Class.forName("android.os.SystemProperties");
                    int slot = (Integer) properties.getMethod("getInt", String.class, int.class)
                            .invoke(null, "persist.radio.simswitch", 1);
                    return slot >= 1 && slot <= 2 ? slot - 1 : 0;
                } catch (ReflectiveOperationException e) {
                    return 0;
                }
            }
            @Override public void setTrmForPhone(int phoneId, int mode) throws RemoteException {
                throw new RemoteException("Modem reset is managed by the radio service");
            }
        };
        public static ITelephonyEx asInterface(IBinder binder) {
            return COMPAT;
        }
    }
}
