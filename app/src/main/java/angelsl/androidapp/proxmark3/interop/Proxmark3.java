package angelsl.androidapp.proxmark3.interop;

public class Proxmark3 {
    private static volatile OutputHandler _handler;

    static {
        System.loadLibrary("proxmark3");
        final int fd = init();
        if (fd >= 0) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    redirThreadWorker(fd);
                }
            }).start();
        }
    }

    public static void setOutputHandler(OutputHandler h) {
        _handler = h;
    }

    public static native int execCommand(String command);

    private static void dispatchOutput(String n) {
        if (_handler != null) {
            _handler.onOutput(n);
        }
    }

    private static native int init();
    private static native void redirThreadWorker(int rfd);

    public interface OutputHandler {
        void onOutput(String output);
    }
}
