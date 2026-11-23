/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */
package com.mediatek.ims.compat;

import android.net.Uri;
import android.telephony.CallQuality;
import android.telephony.ims.*;
import android.os.RemoteException;
import com.android.ims.internal.IImsCallSession;
import com.android.ims.internal.IImsCallSessionListener;

/** Bridges legacy MTK call callbacks to the Android 10 session listener. */
public final class CallSessionListener extends IImsCallSessionListener.Stub {
    private final android.telephony.ims.aidl.IImsCallSessionListener target;

    private CallSessionListener(android.telephony.ims.aidl.IImsCallSessionListener target) {
        this.target = target;
    }

    public static IImsCallSessionListener wrap(
            android.telephony.ims.aidl.IImsCallSessionListener target) {
        return target == null ? null : new CallSessionListener(target);
    }

    @Override public void callSessionProgressing(IImsCallSession session, ImsStreamMediaProfile profile) throws RemoteException {
        target.callSessionProgressing(profile);
    }

    @Override public void callSessionStarted(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionInitiated(profile);
    }

    @Override public void callSessionStartFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionInitiatedFailed(reasonInfo);
    }

    @Override public void callSessionTerminated(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionTerminated(reasonInfo);
    }

    @Override public void callSessionHeld(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionHeld(profile);
    }

    @Override public void callSessionHoldFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionHoldFailed(reasonInfo);
    }

    @Override public void callSessionHoldReceived(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionHoldReceived(profile);
    }

    @Override public void callSessionResumed(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionResumed(profile);
    }

    @Override public void callSessionResumeFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionResumeFailed(reasonInfo);
    }

    @Override public void callSessionResumeReceived(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionResumeReceived(profile);
    }

    @Override public void callSessionMergeStarted(IImsCallSession session, IImsCallSession newSession, ImsCallProfile profile) throws RemoteException {
        target.callSessionMergeStarted(newSession, profile);
    }

    @Override public void callSessionMergeComplete(IImsCallSession session) throws RemoteException {
        target.callSessionMergeComplete(session);
    }

    @Override public void callSessionMergeFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionMergeFailed(reasonInfo);
    }

    @Override public void callSessionUpdated(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionUpdated(profile);
    }

    @Override public void callSessionUpdateFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionUpdateFailed(reasonInfo);
    }

    @Override public void callSessionUpdateReceived(IImsCallSession session, ImsCallProfile profile) throws RemoteException {
        target.callSessionUpdateReceived(profile);
    }

    @Override public void callSessionConferenceExtended(IImsCallSession session, IImsCallSession newSession, ImsCallProfile profile) throws RemoteException {
        target.callSessionConferenceExtended(newSession, profile);
    }

    @Override public void callSessionConferenceExtendFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionConferenceExtendFailed(reasonInfo);
    }

    @Override public void callSessionConferenceExtendReceived(IImsCallSession session, IImsCallSession newSession, ImsCallProfile profile) throws RemoteException {
        target.callSessionConferenceExtendReceived(newSession, profile);
    }

    @Override public void callSessionInviteParticipantsRequestDelivered(IImsCallSession session) throws RemoteException {
        target.callSessionInviteParticipantsRequestDelivered();
    }

    @Override public void callSessionInviteParticipantsRequestFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionInviteParticipantsRequestFailed(reasonInfo);
    }

    @Override public void callSessionRemoveParticipantsRequestDelivered(IImsCallSession session) throws RemoteException {
        target.callSessionRemoveParticipantsRequestDelivered();
    }

    @Override public void callSessionRemoveParticipantsRequestFailed(IImsCallSession session, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionRemoveParticipantsRequestFailed(reasonInfo);
    }

    @Override public void callSessionConferenceStateUpdated(IImsCallSession session, ImsConferenceState state) throws RemoteException {
        target.callSessionConferenceStateUpdated(state);
    }

    @Override public void callSessionUssdMessageReceived(IImsCallSession session, int mode, String ussdMessage) throws RemoteException {
        target.callSessionUssdMessageReceived(mode, ussdMessage);
    }

    @Override public void callSessionHandover(IImsCallSession session, int srcAccessTech, int targetAccessTech, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionHandover(srcAccessTech, targetAccessTech, reasonInfo);
    }

    @Override public void callSessionHandoverFailed(IImsCallSession session, int srcAccessTech, int targetAccessTech, ImsReasonInfo reasonInfo) throws RemoteException {
        target.callSessionHandoverFailed(srcAccessTech, targetAccessTech, reasonInfo);
    }

    @Override public void callSessionMayHandover(IImsCallSession session, int srcAccessTech, int targetAccessTech) throws RemoteException {
        target.callSessionMayHandover(srcAccessTech, targetAccessTech);
    }

    @Override public void callSessionTtyModeReceived(IImsCallSession session, int mode) throws RemoteException {
        target.callSessionTtyModeReceived(mode);
    }

    @Override public void callSessionMultipartyStateChanged(IImsCallSession session, boolean isMultiParty) throws RemoteException {
        target.callSessionMultipartyStateChanged(isMultiParty);
    }

    @Override public void callSessionSuppServiceReceived(IImsCallSession session, ImsSuppServiceNotification suppSrvNotification) throws RemoteException {
        target.callSessionSuppServiceReceived(suppSrvNotification);
    }

    @Override public void callSessionRttModifyRequestReceived(IImsCallSession session, ImsCallProfile callProfile) throws RemoteException {
        target.callSessionRttModifyRequestReceived(callProfile);
    }

    @Override public void callSessionRttModifyResponseReceived(int status) throws RemoteException {
        target.callSessionRttModifyResponseReceived(status);
    }

    @Override public void callSessionRttMessageReceived(String rttMessage) throws RemoteException {
        target.callSessionRttMessageReceived(rttMessage);
    }

    @Override public void callSessionRttAudioIndicatorChanged(ImsStreamMediaProfile profile) throws RemoteException {
        target.callSessionRttAudioIndicatorChanged(profile);
    }

    @Override public void callQualityChanged(CallQuality callQuality) throws RemoteException {
        target.callQualityChanged(callQuality);
    }
}
