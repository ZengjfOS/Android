package com.example.bluetoothbredr;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.IntentFilter;
import android.os.ConditionVariable;
import android.util.Log;

import androidx.core.app.ActivityCompat;

import java.io.IOException;

public class LoopTest extends Thread {

    public ConditionVariable done = null;
    public String TAG = "LoopTest";
    public int i = 0;

    @Override
    public void run() {
        while (true) {
            Log.i(TAG, "loop " + i++);

            done.close();
            done.block();
        }
    }
}
