package com.example.socketclient;

import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Reader;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    String TAG = "SocketClient";
    TextView textView = null;
    Button connect = null;
    Button send = null;

    Socket socket = null;
    ObjectOutputStream oos = null;
    ObjectInputStream ois = null;

    Thread thread = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView = (TextView)findViewById(R.id.helloworld);

        connect = findViewById(R.id.connect);
        send = findViewById(R.id.send);

        thread = new Thread(new Runnable() {

            @Override
            public void run() {

            }
        });
        thread.start();

        connect.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                Log.d(TAG, "connect");
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        //establish socket connection to server
                        try {
                            InetAddress serverAddr = InetAddress.getByName("192.168.43.89");
                            socket = new Socket(serverAddr, 8088);
                            socket.setKeepAlive(true);

                            //write to socket using ObjectOutputStream
                            oos = new ObjectOutputStream(socket.getOutputStream());
                            Log.d(TAG, "send");
                            oos.writeBytes("send to server\n");
                            oos.flush();

                            /*
                            while (true) {
                                Log.d(TAG, "send");
                                oos.writeBytes("send to server\n");
                                oos.flush();

                                Thread.sleep(1000 * 60 * 10);
                            }
                             */
                            ois = new ObjectInputStream(socket.getInputStream());

                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });

        send.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            while (true) {
                                Log.d(TAG, "send");
                                oos.writeBytes("send to server\n");
                                oos.flush();

                                Thread.sleep(1000 * 60 * 10);
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();

            }
        });
    }
}





