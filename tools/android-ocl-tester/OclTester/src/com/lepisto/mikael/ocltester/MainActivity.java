package com.lepisto.mikael.ocltester;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Enumeration;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;

/**
 * To setup android emulator to redirect ports to be visible in host do
 * 
 * telnet localhost 5556
 * redir add tcp:41233:41233
 *
 */
public class MainActivity extends Activity
{
    static final int SocketServerPORT = 41233;

    ServerSocket serverSocket;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        OclTester oclTester = new OclTester();
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView t=(TextView)findViewById(R.id.MainTextContent);
        t.setMovementMethod(new ScrollingMovementMethod());
        
        t.setText("Listening:" + getIpAddress().trim() + ":" + SocketServerPORT + "\n");
        
        t.append(oclTester.getDeviceInfo());
        t.append("\n");

        // valid
        t.append(oclTester.compileWithDevice("1050148873", 
                "kernel void zero_one_or_other(void) {" +
                "}"));

        t.append("\n\n");

        // syntax error
        t.append(oclTester.compileWithDevice("1050148873", 
                "kernel void zero_one_or_other(void) {" +
                        "	I'm Syntax Error." +
                "}"));

        t.append("\n\n\n");

        // compiler fail with intel... silent fail of JNI...
        /*
        t.append(oclTester.compileWithDevice("1050148873", 
        		"kernel void zero_one_or_other(void) {" +
        		"	local uint local_1[1];" +
        		"	local uint local_2[1];" +
        		"	*(local_1 > local_2 ? local_1 : local_2) = 0;" +
        		"}"));
         */

        Thread socketServerThread = new Thread(new SocketServerThread());
        socketServerThread.start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (serverSocket != null) {
            try {
                serverSocket.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    private void appendText(String msg) {
        TextView t=(TextView)findViewById(R.id.MainTextContent);
        t.append(msg);
    }
    
    private class SocketServerThread extends Thread {
        private String message = "";

        int count = 0;
        @Override
        public void run() {

            try {
                serverSocket = new ServerSocket(SocketServerPORT);
                MainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        appendText("I'm waiting here: " + serverSocket.getLocalPort());
                    }
                });

                while (true) {
                    Socket socket = serverSocket.accept();
                    count++;
                    message += "#" + count + " from " + socket.getInetAddress()
                            + ":" + socket.getPort() + "\n";

                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            appendText(message);
                            message = "";
                        }
                    });

                    SocketServerReplyThread socketServerReplyThread = 
                            new SocketServerReplyThread(socket, count);
                    socketServerReplyThread.run();

                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

    }

    private class SocketServerReplyThread extends Thread {
        private String message = "";

        private Socket hostThreadSocket;
        int cnt;

        SocketServerReplyThread(Socket socket, int c) {
            hostThreadSocket = socket;
            cnt = c;
        }

        @Override
        public void run() {
            OutputStream outputStream;
            String msgReply = "Hello from Android, you are #" + cnt;

            try {
                outputStream = hostThreadSocket.getOutputStream();
                PrintStream printStream = new PrintStream(outputStream);
                printStream.print(msgReply);
                printStream.close();

                message += "replayed: " + msgReply + "\n";

                MainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        appendText(message);
                        message = "";
                    }
                });

            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
                message += "Something wrong! " + e.toString() + "\n";
            }

            MainActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    appendText(message);
                }
            });
        }
    }

    private String getIpAddress() {
        String ip = "";
        try {
            Enumeration<NetworkInterface> enumNetworkInterfaces = NetworkInterface.getNetworkInterfaces();
            while (enumNetworkInterfaces.hasMoreElements()) {
                NetworkInterface networkInterface = enumNetworkInterfaces.nextElement();
                Enumeration<InetAddress> enumInetAddress = networkInterface.getInetAddresses();
                while (enumInetAddress.hasMoreElements()) {
                    InetAddress inetAddress = enumInetAddress.nextElement();
                    if (inetAddress.isSiteLocalAddress()) {
                        ip += "SiteLocalAddress: " + inetAddress.getHostAddress() + "\n";
                    }
                }
            }
        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            ip += "Something Wrong! " + e.toString() + "\n";
        }
        return ip;
    }
}
