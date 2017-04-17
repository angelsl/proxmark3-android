package angelsl.androidapp.proxmark3.interop;

import angelsl.androidapp.proxmark3.Config;

import java.io.File;
import java.io.FilenameFilter;

/**
 * Created by angelsl on 20170418.
 */
public final class Device {
    private String _path;
    private String _display;

    private Device(String path) {
        _path = _display = path;
    }

    private Device(String path, String display) {
        _path = path;
        _display = display;
    }

    public String getPath() {
        return _path;
    }

    public String getDisplayName() {
        return _display;
    }

    @Override
    public String toString() {
        return _display;
    }

    public static Device[] findDevices() {
        File dev = new File("/dev");
        // TODO use root to do this; SELinux stops us from seeing /dev
        // TODO so this actually does nothing at the moment
        File[] ttys = dev.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.startsWith("ttyACM");
            }
        });

        int retLen = ttys == null ? 0 : ttys.length;
        Device[] ret = new Device[retLen + (Config.ENABLE_TEST_DEVICE ? 1 : 0)];
        for (int i = 0; i < retLen; ++i) {
            ret[i] = new Device(ttys[i].getAbsolutePath());
        }

        if (Config.ENABLE_TEST_DEVICE) {
            ret[retLen] = new Device("", "Test device");
        }

        return ret;
    }
}
