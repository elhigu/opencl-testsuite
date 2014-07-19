package com.lepisto.mikael.ocltester;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

/**
 * Service process, which calls JNI functions and
 * might crash if JNI side crashes.
 */
public class OclCallService extends Service {
    final private String LOGTAG = "OclCallService";
    
    // Message types for communication with sourceActivity
    public static final int MSG_TYPE_COMMAND = 1;
    public static final int MSG_TYPE_RESULT = 2;
    public static final int MSG_TYPE_ERROR = 3;
    public static final int MSG_TYPE_SERVICE_DISCONNECTED = 4;

    public static final String MSG_KEY_COMMAND = "command";
    public static final String MSG_KEY_RESPONSE = "response";
    public static final String MSG_KEY_DEVICE = "device";
    public static final String MSG_KEY_CODE = "code";
    public static final String MSG_COMMAND_COMPILE = "compile";
    public static final String MSG_COMMAND_INFO = "info";

    final Messenger mMessenger = new Messenger(new IncomingHandler());
    final OclTester oclTester = new OclTester();

    /**
     * Receive command message and send reply.
     */
    @SuppressLint("HandlerLeak")
	class IncomingHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            Log.i(LOGTAG, "Got a message: " + msg.toString());

            if (msg.what != MSG_TYPE_COMMAND) {
                super.handleMessage(msg);
            }

            Bundle input = msg.getData();
            String command = input.getString(MSG_KEY_COMMAND);

            Bundle replyData = new Bundle();
            int replyType = MSG_TYPE_RESULT;

            if (msg.what == MSG_TYPE_COMMAND) {
                if (command.equals(MSG_COMMAND_INFO)) {
                    String devInfo = oclTester.getDeviceInfo();
                    replyData.putString(MSG_KEY_RESPONSE, devInfo);

                } else if (command.equals(MSG_COMMAND_COMPILE)) {
                    String device = input.getString(MSG_KEY_DEVICE);
                    String code =  input.getString(MSG_KEY_CODE);
                    String compileResults = oclTester.compileWithDevice(device, code);
                    replyData.putString(MSG_KEY_RESPONSE, compileResults);

                } else {
                    replyType = MSG_TYPE_ERROR;
                    replyData.putString(MSG_KEY_RESPONSE, "Invalid command: "+ command);
                }
            } else {
                replyType = MSG_TYPE_ERROR;
                replyData.putString(MSG_KEY_RESPONSE, "Invalid message type: "+ msg.what);
            }

            // Reply
            Message reply = Message.obtain(null, replyType);
            reply.setData(replyData);
            try {
                msg.replyTo.send(reply);
            } catch (RemoteException e) {
                Log.i(LOGTAG, "Sending reply from service failed... if this really happens figure out why and what to do.");
                e.printStackTrace();
            }
        }
    }

    public void onCreate() {
        // Wait a bit to be ready to get all debug messages  
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        Log.i(LOGTAG, "Service was started.");
    }
    
    /**
     * Return communication channel for activity.
     */
    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }
}

