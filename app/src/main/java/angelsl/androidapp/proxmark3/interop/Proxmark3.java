package angelsl.androidapp.proxmark3.interop;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Proxmark3 {
    private static OutputHandler _handler;
    private static final ExecutorService _commandQueue;
    private static DeviceInfo _curDevice;

    static {
        System.loadLibrary("pm3jni");
        _commandQueue = Executors.newSingleThreadExecutor();
        new Thread(new Runnable() {
                @Override
                public void run() {
                    redirThreadWorker();
                }
            }).start();
    }

    public static void setOutputHandler(OutputHandler h) {
        _handler = h;
    }

    public static void submitCommand(final String command) {
        _commandQueue.submit(new Runnable() {
            @Override
            public void run() {
                execCommand(command);
                _handler.onCommandCompletion();
            }
        });
    }

    public static void changeDevice(final DeviceInfo device) {
        _commandQueue.submit(new Runnable() {
            @Override
            public void run() {
                _curDevice = device;
                nativeChangeDevice(_curDevice.getPath());
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

    private static native void redirThreadWorker();
    private static native int execCommand(String command);
    private static native void nativeChangeDevice(String path);

    public interface OutputHandler {
        void onOutput(String output);
        void onCommandCompletion();
        void onChangeDevice(DeviceInfo newDevice);
    }
}
