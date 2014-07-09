package com.lepisto.mikael.ocltester;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        TextView t=(TextView)findViewById(R.id.MainTextContent); 
        t.setText(getDeviceInfo());
        t.append(compileWithDevice("1050148873", 
        		"kernel void zero_one_or_other(void) {" +
//        		"	local uint local_1[1];" +
//        		"	local uint local_2[1];" +
//        		"	*(local_1 > local_2 ? local_1 : local_2) = 0;" +
        		"}"));
    }

    static
    {
        System.loadLibrary("ocl-tester");
    }

    private native String getDeviceInfo();
    private native String compileWithDevice(String device, String oclSource);
}
