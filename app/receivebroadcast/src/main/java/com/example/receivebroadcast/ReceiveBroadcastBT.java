package com.example.receivebroadcast;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class ReceiveBroadcastBT extends BroadcastReceiver {
    private String TAG = "ReceiveBroadcastBT";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.e(TAG, "Received BT Broadcast Action: " + intent.getAction());
    }
}
