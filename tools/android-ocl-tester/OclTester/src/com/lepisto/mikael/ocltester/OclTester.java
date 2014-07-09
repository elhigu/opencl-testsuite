package com.lepisto.mikael.ocltester;

public class OclTester {
    static
    {
        System.loadLibrary("ocl-tester");
    }

    public native String getDeviceInfo();
    public native String compileWithDevice(String device, String oclSource);
}
