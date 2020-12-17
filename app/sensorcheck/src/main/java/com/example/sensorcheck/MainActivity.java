package com.example.sensorcheck;

import android.content.Context;
import android.content.res.Resources;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import java.security.Timestamp;

public class MainActivity extends AppCompatActivity {

    public SensorManager sensorManager = null;
    public Sensor als_sensor = null;
    public Sensor ps_sensor = null;
    public Sensor mag_sensor = null;

    public String TAG = "MainActivity";
    public boolean Flag = false;
    public TextView textView = null;

    public int loopIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (!Flag) {
                    textView.setText("start listener:\r\n");
                    int delayTime = 0;
                    switch (loopIndex){
                        case 0:
                            delayTime = SensorManager.SENSOR_DELAY_FASTEST;
                            break;
                        case 1:
                            delayTime = SensorManager.SENSOR_DELAY_GAME;
                            break;
                        case 2:
                            delayTime = SensorManager.SENSOR_DELAY_NORMAL;
                            break;
                        case 3:
                            delayTime = SensorManager.SENSOR_DELAY_UI;
                            break;
                        default:
                            delayTime = SensorManager.SENSOR_DELAY_FASTEST;
                            break;
                    }
                    // sensorManager.registerListener(listener, als_sensor, delayTime);
                    // sensorManager.registerListener(listener, ps_sensor, delayTime);
                    sensorManager.registerListener(listener, mag_sensor, delayTime);
                    Flag = true;

                    if (loopIndex++ > 3)
                        loopIndex = 0;

                } else {
                    sensorManager.unregisterListener(listener);
                    Flag = false;
                }
            }
        });

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        als_sensor = sensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);
        ps_sensor = sensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY);
        mag_sensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        textView = (TextView)findViewById(R.id.debugInfo);
        textView.setMaxLines(100);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private SensorEventListener listener = new SensorEventListener() {

        long preTimestamp = 0;
        long curTimestamp = 0;

        @Override
        public void onSensorChanged(SensorEvent event) {
            curTimestamp = System.currentTimeMillis();

            String outputStr = "" + System.currentTimeMillis() + ": ";
            for (float value: event.values) {
                outputStr +=  " " + value;
            }

            if (preTimestamp != 0)
                outputStr +=  " , diff time: " + (curTimestamp - preTimestamp);

            outputStr += "\r\n";

            if (textView != null)
                textView.append(outputStr);

            Log.e(TAG, outputStr);
            preTimestamp = curTimestamp;
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {

        }
    };
}
