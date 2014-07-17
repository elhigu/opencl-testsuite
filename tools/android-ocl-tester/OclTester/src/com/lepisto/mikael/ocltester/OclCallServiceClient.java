package com.lepisto.mikael.ocltester;

import java.util.concurrent.SynchronousQueue;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
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
    
    // queue to wait for response from service
    SynchronousQueue<String> msgQueue = new SynchronousQueue<String>();

    //
    // MESSAGE PROCESSOR FOR OclCallServiceClient
    //
    @SuppressLint("HandlerLeak")
    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            try {
                if (msg.what == OclCallService.MSG_TYPE_RESULT) {
                    Bundle msgData = msg.getData();
                    String response = msgData.getString(OclCallService.MSG_KEY_RESPONSE);
                    System.out.println("Going to write message to queue: " + response);
                    msgQueue.put(response);
                    System.out.println("=== writing done ===");
                } else {
                    Bundle msgData = msg.getData();
                    System.out.println("Going to write error: " + "Got error or something from service: " + msg.toString() + " Response: " + msgData.toString());
                    msgQueue.put("Got error or something from service: " + msg.toString() + " Response: " + msgData.toString());
                    System.out.println("=== writing done ===");
                }
            } catch (InterruptedException e) {
                System.out.println("Got exception in ClinetHandler!");
                // TODO what to do if this fails? it should not...
                e.printStackTrace();
            }
        }
    }
    final Messenger messageReceiver = new Messenger(new IncomingHandler());

    
    OclCallServiceClient(Activity boundTo) {
        sourceActivity = boundTo;
    }

    public String getDeviceInfo() {
        
        // TODO: wait that initialization is complete
        
        Message msg = Message.obtain(null, OclCallService.MSG_TYPE_COMMAND);
        Bundle data = new Bundle();
        msg.setData(data);
        data.putString(OclCallService.MSG_KEY_COMMAND, "info");
        msg.replyTo = messageReceiver;
        
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            System.out.println("Sending message failed. Check out why and how to handle it.");
            e.printStackTrace();
        }
        
        // TODO: throw error if not getting result in time or if message 
        //       service was broken...

        System.out.println("Waiting for reply....");
        String response = null;
        try {
            response = msgQueue.take();
        } catch (InterruptedException e) {
            System.out.println("Waiting for response failed for some reason. Figure out what to do with it.");
            e.printStackTrace();
        }
        System.out.println("Gotit: " + response);
        
        // TODO: check if message was correct or if it is some other 
        //       broken thing...
        
        return response;
    }

    /**
     * Class for interacting with the main interface of the service.
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        
        public void onServiceConnected(ComponentName className, IBinder service) {
            mService = new Messenger(service);
        }

        public void onServiceDisconnected(ComponentName className) {
            Message msg = Message.obtain(null, OclCallService.MSG_TYPE_SERVICE_DISCONNECTED);
            try {
                // TODO: send message about failure...
                messageReceiver.send(msg);
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
    }

    void doUnbindService() {
        if (mService != null) {
            // Detach our existing connection.
            sourceActivity.unbindService(mConnection);
            mService = null;
        }
    }
}
