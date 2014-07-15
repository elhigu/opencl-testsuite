package com.lepisto.mikael.ocltester;
import android.app.Activity;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;

/**
 * To setup android emulator to redirect ports to be visible in host do
 * 
 * TODO: DIDNT WORK. MAYBE OCLCALLSERVICE COULD WORK BETTER!
 */
public class OclCallService extends Service {

    @Override
    public IBinder onBind(Intent intent) {
        OclTester oclTester = new OclTester();
        // if crash, won't be finished...
        // oclTester.compileWithDevice("1050148873", "kernel void zero_one_or_other(void) {local uint local_1[1];local uint local_2[1];*(local_1 > local_2 ? local_1 : local_2) = 0;}");
        return null;
    }

}

