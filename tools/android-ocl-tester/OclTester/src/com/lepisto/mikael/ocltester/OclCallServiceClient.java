package com.lepisto.mikael.ocltester;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.TimeUnit;

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
import android.util.Log;

/**
 * Communicates with OclCallService and recognize if it crashed etc.
 */
public class OclCallServiceClient {    
    private static final String LOGTAG = "OclServiceClient";
    Messenger mService = null;
    boolean mIsBound;

    // Activity is needed to be able to bind service with it
    private Activity sourceActivity;
    
    // queue to wait for response from service
    private SynchronousQueue<String> msgQueue = new SynchronousQueue<String>();

    // Latch to use for waiting until service is connected
    private CountDownLatch initLock = null;

    //
    // MESSAGE PROCESSOR FOR OclCallServiceClient
    //
    @SuppressLint("HandlerLeak")
    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            try {
                String response = "";
                if (msg.what == OclCallService.MSG_TYPE_RESULT) {
                    Bundle msgData = msg.getData();
                    response = msgData.getString(OclCallService.MSG_KEY_RESPONSE);
                } else if (msg.what == OclCallService.MSG_TYPE_SERVICE_DISCONNECTED) {
                    response = "false:Error: OclCallService disconnected unexpectedly! Probably OCL driver crash.";
                } else {
                    Bundle msgData = msg.getData();
                    response = "Got error or something from service: " + 
                            msg.toString() + " Response: " + msgData.toString();
                }
                
                Log.i(LOGTAG, "Going to write message to queue: " + response);
                msgQueue.put(response);
                Log.i(LOGTAG, "=== writing done ===");

            } catch (InterruptedException e) {
                Log.i(LOGTAG, "Got exception in ClinetHandler!");
                // TODO what to do if this fails? it should not...
                e.printStackTrace();
            }
        }
    }
    final Messenger messageReceiver = new Messenger(new IncomingHandler());

    
    OclCallServiceClient(Activity boundTo) {
        sourceActivity = boundTo;
        doBindService();
    }

    /**
     * Disconnect client from service.
     * 
     * Would be nice to call this from activity onDestroy()
     */
    public void release() {
        Log.i(LOGTAG, "Unbinding OclCallService...");
        doUnbindService();
    }
    
    /**
     * TODO: Refactor code with compileWithDevice...
     */
    public String getDeviceInfo() {
        // TODO: make sure that oclClient is not called from main thread of program
        //       which does event handling (wait will block handling and we are screwed).. 
        waitForServiceToBeReady();
        
        Message msg = Message.obtain(null, OclCallService.MSG_TYPE_COMMAND);
        Bundle data = new Bundle();
        msg.setData(data);
        data.putString(OclCallService.MSG_KEY_COMMAND, "info");
        msg.replyTo = messageReceiver;
        
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            Log.i("OclServiceClient", "Sending message failed. Check out why and how to handle it.");
            e.printStackTrace();
        }

        Log.i("OclServiceClient", "Waiting for reply....");
        String response = null;
        try {
            response = msgQueue.take();
        } catch (InterruptedException e) {
            Log.i("OclServiceClient", "Waiting for response failed for some reason. Figure out what to do with it.");
            e.printStackTrace();
        }
        Log.i("OclServiceClient", "Gotit: " + response);

        return response;
    }

    
    public String compileWithDevice(String deviceId, String code) {

        // TODO: make sure that oclClient is not called from main thread of program
        //       which does event handling (wait will block handling and we are screwed).. 
        waitForServiceToBeReady();
        
        Message msg = Message.obtain(null, OclCallService.MSG_TYPE_COMMAND);
        Bundle data = new Bundle();
        msg.setData(data);

        data.putString(OclCallService.MSG_KEY_COMMAND, OclCallService.MSG_COMMAND_COMPILE);
        data.putString(OclCallService.MSG_KEY_DEVICE, deviceId);
        data.putString(OclCallService.MSG_KEY_CODE, code);
        msg.replyTo = messageReceiver;

        try {
            mService.send(msg);
        } catch (RemoteException e) {
            Log.i("OclServiceClient", "Sending message failed. If this happens, check out why and how to handle it.");
            e.printStackTrace();
        }

        Log.i("OclServiceClient", "Waiting for reply....");
        String response = null;
        try {
            response = msgQueue.take();
        } catch (InterruptedException e) {
            Log.i("OclServiceClient", "Waiting for response failed for some reason. Figure out what to do with it.");
            e.printStackTrace();
        }

        Log.i("OclServiceClient", "Gotit: " + response);
        return response;
    }

    /** 
     * Waits until service is connected
     */
    private void waitForServiceToBeReady() {

        // if not bound try to bind again
        doBindService();

        // wait that initialization is complete
        Log.i(LOGTAG, "Waiting for connection to OclCallService...");
        try {
            // If this is called from Main thread, it will hang, since 
            // ServiceConnection call backs are called in main thread
            while (!initLock.await(10000, TimeUnit.MILLISECONDS)) {
                Log.i(LOGTAG, "Still waiting...");
            }
        } catch (InterruptedException e1) {
            Log.i(LOGTAG, "Countdown wait was interrupted... figure out why and what to do if this actually happens.");
            e1.printStackTrace();
        }
        Log.i(LOGTAG, "Connected! Continue using the service.");
    }

    /**
     * Class for interacting with the main interface of the service.
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        
        public void onServiceConnected(ComponentName className, IBinder service) {
            Log.i(LOGTAG, "Service connected!");
            mService = new Messenger(service);
            initLock.countDown();
        }

        public void onServiceDisconnected(ComponentName className) {
            Message msg = Message.obtain(null, OclCallService.MSG_TYPE_SERVICE_DISCONNECTED);
            try {
                messageReceiver.send(msg);
            } catch (RemoteException e) {
                Log.i(LOGTAG, "Could not send response to client... If this happens really need to figure out what to do.");
                e.printStackTrace();
            }
            mService = null;
        }
    };

    private void doBindService() {
        // trust that mService was set to null when disconnect was called...
        if (mService == null) {
            initLock =  new CountDownLatch(1);
            Log.i(LOGTAG, "Going to bind service...");
            sourceActivity.bindService(
                    new Intent(sourceActivity, OclCallService.class), 
                    mConnection, Context.BIND_AUTO_CREATE);
            Log.i(LOGTAG, "binding ready.");
        }
    }

    private void doUnbindService() {
        if (mService != null) {
            // Detach our existing connection.
            sourceActivity.unbindService(mConnection);
            mService = null;
        }
    }
}
