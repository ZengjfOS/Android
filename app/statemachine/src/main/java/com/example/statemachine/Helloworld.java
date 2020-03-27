package com.example.statemachine;

import android.os.Message;
import android.util.Log;

class Helloworld extends StateMachine {

    private static final String TAG = "Helloworld";

    public static final int CMD_1 = 1;
    public static final int CMD_2 = 2;
    public static final int CMD_3 = 3;
    public static final int CMD_4 = 4;
    public static final int CMD_5 = 5;

    public static Helloworld makeHelloworld() {
        Log.d(TAG, "makeHelloworld E");
        Helloworld sm = new Helloworld("hsm1");
        sm.start();
        Log.d(TAG, "makeHelloworld X");
        return sm;
    }

    Helloworld(String name) {
        super(name);
        Log.d(TAG, "ctor E");

        // 构造状态树
        addState(mP1);
          addState(mS1, mP1);
          addState(mS2, mP1);
        addState(mP2);

        // 设置初始状态
        setInitialState(mS1);
        Log.d(TAG, "ctor X");
    }

    class P1 extends State {
        @Override
        public void enter() {
            Log.d(TAG, "mP1.enter");
        }
        @Override
        public boolean processMessage(Message message) {
            boolean retVal;
            Log.d(TAG, "mP1.processMessage what=" + message.what);
            switch(message.what) {
                case CMD_2:
                    // CMD_2 will arrive in mS2 before CMD_3
                    sendMessage(obtainMessage(CMD_3));
                    deferMessage(message);
                    transitionTo(mS2);
                    retVal = HANDLED;
                    break;
                default:
                    // Any message we don't understand in this state invokes unhandledMessage
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
        @Override
        public void exit() {
            Log.d(TAG, "mP1.exit");
        }
    }

    class S1 extends State {
        @Override
        public void enter() {
            Log.d(TAG, "mS1.enter");
        }
        @Override
        public boolean processMessage(Message message) {
            Log.d(TAG, "S1.processMessage what=" + message.what);
            if (message.what == CMD_1) {
                // Transition to ourself to show that enter/exit is called
                transitionTo(mS1);
                return HANDLED;
            } else {
                // Let parent process all other messages
                return NOT_HANDLED;
            }
        }
        @Override
        public void exit() {
            Log.d(TAG, "mS1.exit");
        }
    }

    class S2 extends State {
        @Override
        public void enter() {
            Log.d(TAG, "mS2.enter");
        }
        @Override
        public boolean processMessage(Message message) {
            boolean retVal;
            Log.d(TAG, "mS2.processMessage what=" + message.what);
            switch(message.what) {
                case(CMD_2):
                    sendMessage(obtainMessage(CMD_4));
                    retVal = HANDLED;
                    break;
                case(CMD_3):
                    deferMessage(message);
                    transitionTo(mP2);
                    retVal = HANDLED;
                    break;
                default:
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
        @Override
        public void exit() {
            Log.d(TAG, "mS2.exit");
        }
    }


    class P2 extends State {
        @Override
        public void enter() {
            Log.d(TAG, "mP2.enter");
            sendMessage(obtainMessage(CMD_5));
        }
        @Override
        public boolean processMessage(Message message) {
            Log.d(TAG, "P2.processMessage what=" + message.what);
            switch(message.what) {
                case(CMD_3):
                    break;
                case(CMD_4):
                    break;
                case(CMD_5):
                    transitionToHaltingState();
                    break;
            }
            return HANDLED;
        }
        @Override
        public void exit() {
            Log.d(TAG, "mP2.exit");
        }
    }

    @Override
    public void onHalting() {
        Log.d(TAG, "halting");
        synchronized (this) {
            this.notifyAll();
        }
    }

    P1 mP1 = new P1();
    S1 mS1 = new S1();
    S2 mS2 = new S2();
    P2 mP2 = new P2();
}

