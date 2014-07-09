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
    }

    static
    {
        System.loadLibrary("ocl-tester");
    }

    private native String getDeviceInfo();
}
