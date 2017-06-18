package angelsl.androidapp.proxmark3.interop;

import android.content.Context;

import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Proxmark3 {
    private static OutputHandler _handler;
    private static final ExecutorService _commandQueue;
    private static DeviceInfo _curDevice;
    private static volatile boolean _init = false;
    private static final String RELAYD_NAME = "libpm3relayd.so";

    static {
        System.loadLibrary("pm3jni");
        _commandQueue = Executors.newSingleThreadExecutor();
        new Thread(new Runnable() {
                @Override
                public void run() {
                    jniStdoutWorker();
                }
            }).start();
    }

    public static void init(Context c) {
        if (!_init) {
            _init = true;
            jniSetRelaydPath(new File(c.getApplicationInfo().nativeLibraryDir, RELAYD_NAME).getAbsolutePath());
        }
    }

    public static void setOutputHandler(OutputHandler h) {
        _handler = h;
    }

    public static void execCommand(final String command) {
        _commandQueue.submit(new Runnable() {
            @Override
            public void run() {
                jniExecCommand(command);
                _handler.onCommandCompletion();
            }
        });
    }

    public static void changeDevice(final DeviceInfo device) {
        _commandQueue.submit(new Runnable() {
            @Override
            public void run() {
                _curDevice = device;
                jniChangeDevice(_curDevice.getPath());
                _handler.onChangeDevice(device);
            }
        });
    }

    // Used by C-side to send std{out,err} into the GUI
    @SuppressWarnings("unused")
    private static void dispatchOutput(String n) {
        if (_handler != null) {
            _handler.onOutput(n);
        }
    }

    private static native void jniStdoutWorker();
    private static native int jniExecCommand(String command);
    private static native void jniChangeDevice(String path);
    private static native void jniSetRelaydPath(String path);

    public interface OutputHandler {
        void onOutput(String output);
        void onCommandCompletion();
        void onChangeDevice(DeviceInfo newDevice);
    }
}
