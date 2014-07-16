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

/**
 * To setup android emulator to redirect ports to be visible in host do
 */
public class OclCallService extends Service {

    /**
     * Inter Process Communication...
     */
    @SuppressLint("HandlerLeak")
	class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Bundle input = msg.getData();
            String command = input.getString("command");
            Message reply = new Message();
            Bundle replyData = new Bundle();

            if (command.equals("info")) {
                String devInfo = oclTester.getDeviceInfo();
                replyData.putString("response", devInfo);
                reply.setData(replyData);
            } else if (command.equals("compile")) {
                // TODO: get code from input.getString("code");...
                // if crash, won't be finished..
                String compileResults = oclTester.compileWithDevice("1050148873", "kernel void zero_one_or_other(void) {local uint local_1[1];local uint local_2[1];*(local_1 > local_2 ? local_1 : local_2) = 0;}");
                replyData.putString("response", compileResults);
                reply.setData(replyData);
            } else {
                super.handleMessage(msg);
            }

            try {
                msg.replyTo.send(reply);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }
    
    final Messenger mMessenger = new Messenger(new IncomingHandler());
    final OclTester oclTester = new OclTester();

    /**
     * When binding to the service, we return an interface to our messenger
     * for sending messages to the service.
     */
    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }
/*
    NotificationManager mNM;
    
    @Override
    public void onCreate() {
        // mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
        // mNM.notify(R.string.remote_service_started, notification);
        // showNotification();
    }

    @Override
    public void onDestroy() {
        // Cancel the persistent notification.
        // mNM.cancel(R.string.remote_service_started);
        // Tell the user we stopped.
        // Toast.makeText(this, R.string.remote_service_stopped, Toast.LENGTH_SHORT).show();
    }
    
*/
    /**
     * Show a notification while this service is running.
    private void showNotification() {
        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = getText(R.string.remote_service_started);

        // Set the icon, scrolling text and timestamp
        Notification notification = new Notification(R.drawable.stat_sample, text,
                System.currentTimeMillis());

        // The PendingIntent to launch our activity if the user selects this notification
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                new Intent(this, Controller.class), 0);

        // Set the info for the views that show in the notification panel.
        notification.setLatestEventInfo(this, getText(R.string.remote_service_label),
                       text, contentIntent);

        // Send the notification.
        // We use a string id because it is a unique number.  We use it later to cancel.
        mNM.notify(R.string.remote_service_started, notification);
    }
     */
}

