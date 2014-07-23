package com.lepisto.mikael.ocltester;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.TextView;

/**
 * To setup android emulator to redirect ports to be visible in host do
 * 
 * telnet localhost 5554
 * redir add tcp:41233:41233
 *
 * To test commands with netcat
 * 
 * echo '{ "command" : "info" }' | nc localhost 41233
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) {}" }' | nc localhost 41233
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) { Syntax Error. }" }' | nc localhost 41233
 * 
 * This test actually crashes on the JNI side (should still report error)
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void zero_one_or_other(void) {local uint local_1[1];local uint local_2[1];*(local_1 > local_2 ? local_1 : local_2) = 0;}" }' | nc localhost 41233
 */
public class OclTesterActivity extends Activity
{
    static final String LOGTAG = "OclTesterActivity";
    static final int SocketServerPORT = 41233;

    ServerSocket serverSocket;
    OclCallServiceClient oclClient;

    ConcurrentLinkedQueue<String> messagesFromOtherThreads = new ConcurrentLinkedQueue<String>();

    //
    // Main Activity
    //
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // Wait a bit to be ready to get all debug messages  
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        oclClient = new OclCallServiceClient(this);

        setContentView(R.layout.main);
        TextView t=(TextView)findViewById(R.id.MainTextContent);
        t.setMovementMethod(new ScrollingMovementMethod());
        t.setText("Listening:" + getIpAddress().trim() + ":" + SocketServerPORT + "\n");

        // Start listening only after service if bound
        Thread socketServerThread = new Thread(new SocketServerThread());
        socketServerThread.start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        oclClient.release();

        if (serverSocket != null) {
            try {
                serverSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    
    private void appendText(String msg) {
        TextView t=(TextView)findViewById(R.id.MainTextContent);
        if (t.getText().length() > 100000) {
            t.setText(msg);
        } else {
            t.append(msg);
        }
    }
    
    /**
     * Add message to queue and tell UI thread to put it on 
     */
    public void log(String message) {
        Log.i(LOGTAG, "Putting message to Queue for showing in UI thread: " + message);

        messagesFromOtherThreads.add(message.trim() + "\n");
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String msg = messagesFromOtherThreads.remove();
                Log.i(LOGTAG, "Putting msg to UI: " + msg);
                appendText(msg);
            }
        });
    }

    //
    // SOCKET COMMUNICATION START HERE....
    //

    /**
     * Listen new connections to execute commands with OclTester.
     */
    private class SocketServerThread extends Thread {
        private final String LOGTAG = "SocketServerThread";

        @Override
        public void run() {

            testOclClient();

            Log.i(LOGTAG, "Starting TCP connection listener thread.");
            try {
                
                serverSocket = new ServerSocket(SocketServerPORT);
                while (true) {
                    Socket socket = serverSocket.accept();
                    SocketServerReplyThread socketServerReplyThread = 
                            new SocketServerReplyThread(socket);
                    socketServerReplyThread.run();
                }
            } catch (IOException e) {
                e.printStackTrace();
                OclTesterActivity.this.log("There was error creating response socket...\n");
            }
        }

        private void testOclClient() {
            // 
            // Test some simple OpenCL cases to make sure OclClient is connected
            // to service. These cannot be called in UI thread.
            // 
            OclTesterActivity.this.log("Running simple tests with OclClient.\n");
            String devInfo = oclClient.getDeviceInfo();

            // For some reason 
            OclTesterActivity.this.log("Info:\n" + devInfo + "\n\n");

            String compileResult = oclClient.compileWithDevice(
                    "1050148873", 
                    "kernel void zero_one_or_other(void) { }");

            // valid compile test case
            OclTesterActivity.this.log("Compile test #1: " + compileResult);

            // syntax error
            OclTesterActivity.this.log(
                "Compile test #2: " + 
                oclClient.compileWithDevice("1050148873", 
                    "kernel void zero_one_or_other(void) {" +
                    "   I'm Syntax Error." +
                    "}") + 
                "\n\n"
            );

            // compiler fail with intel... silent fail of JNI...
            OclTesterActivity.this.log(
               "Compile test #3: " +                     
               oclClient.compileWithDevice("1050148873", 
                    "kernel void zero_one_or_other(void) {" +
                    "   local uint local_1[1];" +
                    "   local uint local_2[1];" +
                    "   *(local_1 > local_2 ? local_1 : local_2) = 0;" +
                    "}") + 
               "\n\n"
            );

            OclTesterActivity.this.log("Test getting devinfo again, should work even after driver crash:\n" + oclClient.getDeviceInfo() + "\n\n");
        }
    }

    /**
     * Read input of connection, execute OclTester things and send response.
     */
    private class SocketServerReplyThread extends Thread {
        final private String LOGTAG = "SocketServerReplyThread";
        private Socket hostThreadSocket;
        
        SocketServerReplyThread(Socket socket) {
            hostThreadSocket = socket;
        }
        
        @Override
        public void run() {
            OutputStream outputStream;
            InputStream inputStream;
            
            Log.i(LOGTAG, "Started new thread to serve TCP connection.");
            
            try {
                inputStream = hostThreadSocket.getInputStream();
                // one million bytes of code max limit for now.
                byte[] buffer = new byte[1000000];
                inputStream.read(buffer);
                
                JSONObject retVal = new JSONObject();
                
                try {
                    String commandStr = new String(buffer, "UTF-8");
                    OclTesterActivity.this.log("Got command: " + commandStr + "\n");
                    
                    JSONObject json = new JSONObject(commandStr);
                    String command = json.getString("command");

                    if (command.equals("info")) {
                        retVal.put("status", true);
                        retVal.put("output", oclClient.getDeviceInfo());
                    
                    } else if (command.equals("compile")) {
                        String device = json.getString("device");
                        String code = json.getString("code");

                        Log.i(LOGTAG,"Starting to compile: " + device + ":" + code + "\n");
                        String compileStatus = oclClient.compileWithDevice(device, code);
                        Log.i(LOGTAG, "Return: " + compileStatus + "\n");

                        int separator = compileStatus.indexOf(':');
                        boolean status = Boolean.parseBoolean(compileStatus.substring(0, separator));
                        String output = compileStatus.substring(separator+1);
                        
                        Log.i(LOGTAG, "Parsed status:" + status + " Parced output:" + output + "\n");
                        retVal.put("status", status);
                        retVal.put("output", output);

                    } else {
                        retVal.put("status", false);
                        retVal.put("output", "Unknown command: " + command);
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                    try {
                        retVal.put("status", false);
                        retVal.put("output", "Could not parse command or arguments.");
                    } catch (JSONException e1) {
                        e1.printStackTrace();
                    }
                }

                outputStream = hostThreadSocket.getOutputStream();
                PrintStream printStream = new PrintStream(outputStream);
                String retStr = retVal.toString() + "\n";
                OclTesterActivity.this.log("Returning: " + retStr);

                printStream.print(retStr);
                printStream.close();

            } catch (IOException e) {
                e.printStackTrace();
                OclTesterActivity.this.log("There was error when executing command...\n");
            }
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
            e.printStackTrace();
            ip += "Getting IP failed: " + e.toString() + "\n";
        }
        return ip;
    }
}
