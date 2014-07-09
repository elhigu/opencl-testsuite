package com.lepisto.mikael.ocltester;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity
{
	// ServerSocket serverSocket;
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
    	OclTester oclTester = new OclTester();
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView t=(TextView)findViewById(R.id.MainTextContent); 
        t.setText(oclTester.getDeviceInfo());

        t.append("\n\n");

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

//        Thread socketServerThread = new Thread(new SocketServerThread());
//        socketServerThread.start();
    }

    /*
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
    */
}
