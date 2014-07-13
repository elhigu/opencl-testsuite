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

import org.json.JSONException;
import org.json.JSONObject;

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
 * To test commands with netcat
 * 
 * echo '{ "command" : "info" }' | nc localhost 41233
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) {}" }' | nc localhost 41233
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) { Syntax Error. }" }' | nc localhost 41233
 * 
 * This test actually crashes on the JNI side (should still report error)
 * echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void zero_one_or_other(void) {local uint local_1[1];local uint local_2[1];*(local_1 > local_2 ? local_1 : local_2) = 0;}" }' | nc localhost 41233
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
        t.append("Compile test #1: " + oclTester.compileWithDevice("1050148873", 
                "kernel void zero_one_or_other(void) {" +
                "}"));

        t.append("\n\n");

        // syntax error
        t.append("Compile test #2: " +oclTester.compileWithDevice("1050148873", 
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
                e.printStackTrace();
            }
        }
    }

    private void appendText(String msg) {
        TextView t=(TextView)findViewById(R.id.MainTextContent);
        t.append(msg);
    }

    /**
     * Listen new connections to execute commands with OclTester.
     */
    private class SocketServerThread extends Thread {
        @Override
        public void run() {
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
                MainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        appendText("There was error creating response socket...\n");
                    }
                });
            }
        }
    }

    /**
     * Read input of connection, execute OclTester things and send response.
     */
    private class SocketServerReplyThread extends Thread {
        private Socket hostThreadSocket;
        private String tempMessage = "";
        
        SocketServerReplyThread(Socket socket) {
            hostThreadSocket = socket;
        }

        @Override
        public void run() {
            OutputStream outputStream;
            InputStream inputStream;

            try {
                inputStream = hostThreadSocket.getInputStream();
                // one million bytes of code max limit for now.
                byte[] buffer = new byte[1000000];
                inputStream.read(buffer);
                
                JSONObject retVal = new JSONObject();
                
                try {
                    OclTester oclTester = new OclTester();
                    String commandStr = new String(buffer, "UTF-8");
                    log("Got command: " + commandStr + "\n");
                    
                    JSONObject json = new JSONObject(commandStr);
                    String command = json.getString("command");

                    if (command.equals("info")) {
                        retVal.put("status", true);
                        retVal.put("output", oclTester.getDeviceInfo());
                    
                    } else if (command.equals("compile")) {
                        String device = json.getString("device");
                        String code = json.getString("code");

                        log("Starting to compile: " + device + ":" + code + "\n");
                        String compileStatus = oclTester.compileWithDevice(device, code);
                        log("Return: " + compileStatus + "\n");

                        int separator = compileStatus.indexOf(':');
                        boolean status = Boolean.parseBoolean(compileStatus.substring(0, separator));
                        String output = compileStatus.substring(separator+1);
                        log("Returning:" + status + " output:" + output + "\n");

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
                printStream.print(retVal.toString() + "\n");
                printStream.close();

            } catch (IOException e) {
                e.printStackTrace();
                log("There was error when executing command...\n");
            }
        }

        private void log(String msg) {
            tempMessage = msg;
            MainActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    appendText(tempMessage);
                    tempMessage = "";
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
            e.printStackTrace();
            ip += "Getting IP failed: " + e.toString() + "\n";
        }
        return ip;
    }
}
