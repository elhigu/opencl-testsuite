package com.lepisto.mikael.ocltester;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;

/**
 * Communicates with OclCallService and recognize if it crashed etc.
 */
public class OclCallServiceClient {

    Messenger mService = null;
    boolean mIsBound;

    // Activity is needed to be able to bind service with it
    private Activity sourceActivity;
    private Messenger sourceMessenger;
    
    OclCallServiceClient(Activity boundTo, Messenger msgChannel) {
        sourceActivity = boundTo;
        sourceMessenger = msgChannel;
    }

    public boolean getInfo() {
        Message msg = Message.obtain(null, OclCallService.MSG_TYPE_COMMAND);
        Bundle data = new Bundle();
        msg.setData(data);
        data.putString(OclCallService.MSG_KEY_COMMAND, "info");
        msg.replyTo = sourceMessenger;

        if (mService == null) {
            return false;
        }
        
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    /**
     * Class for interacting with the main interface of the service.
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        
        public void onServiceConnected(ComponentName className, IBinder service) {
            mService = new Messenger(service);
            getInfo();
        }

        public void onServiceDisconnected(ComponentName className) {
            Message msg = Message.obtain(null, OclCallService.MSG_TYPE_SERVICE_DISCONNECTED);
            try {
                sourceMessenger.send(msg);
            } catch (RemoteException e) {
                // TODO: we could call some known good callback too
                //       directly from activity.. this should never fail though.
                e.printStackTrace();
            }
            mService = null;
        }
    };

    void doBindService() {
        sourceActivity.bindService(
                new Intent(sourceActivity, OclCallService.class), 
                mConnection, Context.BIND_AUTO_CREATE);
        mIsBound = true;
    }

    void doUnbindService() {
        if (mIsBound) {
            // Detach our existing connection.
            sourceActivity.unbindService(mConnection);
            mIsBound = false;
        }
    }
}
