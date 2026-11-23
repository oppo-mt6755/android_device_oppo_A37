/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */
package com.mediatek.ims.compat;

import android.net.Uri;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.telephony.ims.ImsReasonInfo;
import com.android.ims.internal.IImsRegistrationListener;

/* Q registers independent capability and registration callbacks on the legacy service. */
public final class RegistrationListeners extends IImsRegistrationListener.Stub {
    private final RemoteCallbackList<IImsRegistrationListener> listeners = new RemoteCallbackList<>();
    private interface Event { void send(IImsRegistrationListener listener) throws RemoteException; }

    public static IImsRegistrationListener add(IImsRegistrationListener previous,
            IImsRegistrationListener listener) {
        RegistrationListeners callbacks = previous instanceof RegistrationListeners
                ? (RegistrationListeners) previous : new RegistrationListeners();
        if (listener == null) {
            callbacks.listeners.kill();
            return null;
        }
        if (previous != null && previous != callbacks) callbacks.listeners.register(previous);
        callbacks.listeners.register(listener);
        return callbacks;
    }

    private synchronized void dispatch(Event event) {
        int count = listeners.beginBroadcast();
        try {
            for (int i = 0; i < count; i++) {
                try {
                    event.send(listeners.getBroadcastItem(i));
                } catch (RemoteException ignored) {
                    // RemoteCallbackList removes dead processes.
                }
            }
        } finally {
            listeners.finishBroadcast();
        }
    }

    @Override public void registrationConnected() {
        registrationConnectedWithRadioTech(14); // The supported native bearer is LTE.
    }
    @Override public void registrationProgressing() { registrationProgressingWithRadioTech(14); }
    @Override public void registrationConnectedWithRadioTech(int tech) {
        dispatch(listener -> listener.registrationConnectedWithRadioTech(tech));
    }
    @Override public void registrationProgressingWithRadioTech(int tech) {
        dispatch(listener -> listener.registrationProgressingWithRadioTech(tech));
    }
    @Override public void registrationDisconnected(ImsReasonInfo reason) {
        dispatch(listener -> listener.registrationDisconnected(reason));
    }
    @Override public void registrationResumed() { dispatch(IImsRegistrationListener::registrationResumed); }
    @Override public void registrationSuspended() { dispatch(IImsRegistrationListener::registrationSuspended); }
    @Override public void registrationServiceCapabilityChanged(int serviceClass, int event) {
        dispatch(listener -> listener.registrationServiceCapabilityChanged(serviceClass, event));
    }
    @Override public void registrationFeatureCapabilityChanged(int serviceClass, int[] enabled,
            int[] disabled) {
        dispatch(listener -> listener.registrationFeatureCapabilityChanged(serviceClass, enabled, disabled));
    }
    @Override public void voiceMessageCountUpdate(int count) {
        dispatch(listener -> listener.voiceMessageCountUpdate(count));
    }
    @Override public void registrationAssociatedUriChanged(Uri[] uris) {
        dispatch(listener -> listener.registrationAssociatedUriChanged(uris));
    }
    @Override public void registrationChangeFailed(int tech, ImsReasonInfo reason) {
        dispatch(listener -> listener.registrationChangeFailed(tech, reason));
    }
}
