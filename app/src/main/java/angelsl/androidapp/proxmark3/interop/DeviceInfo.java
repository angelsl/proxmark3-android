package angelsl.androidapp.proxmark3.interop;

import android.util.Log;

import angelsl.androidapp.proxmark3.Config;

import java.io.BufferedReader;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public final class DeviceInfo {
    private final String _path;
    private final String _display;

    private DeviceInfo(String path) {
        _path = _display = path;
    }

    private DeviceInfo(String path, String display) {
        _path = path;
        _display = display;
    }

    public String getPath() {
        return _path;
    }

    @Override
    public String toString() {
        return _display;
    }

    public static DeviceInfo[] findDevices() {
        File dev = new File("/dev");
        File[] ttys = dev.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.startsWith("ttyACM");
            }
        });

        String[] devPaths;
        if (ttys == null) {
            try {
                ArrayList<String> result = new ArrayList<>();
                Process ls = Runtime.getRuntime().exec(new String[]{"su", "root", "ls", "/dev"});
                BufferedReader br =
                        new BufferedReader(new InputStreamReader(ls.getInputStream()));
                try {
                    String readLine;

                    while ((readLine = br.readLine()) != null) {
                        if (!(readLine = readLine.trim()).isEmpty() && readLine.startsWith("ttyACM")) {
                            result.add("/dev/" + readLine);
                        }
                    }
                } finally {
                    br.close();
                }
                devPaths = result.toArray(new String[0]);
            } catch (IOException ioex) {
                devPaths = new String[0];
                Log.e("Proxmark3", "Exception while `su root ls /dev`", ioex);
            }
        } else {
            devPaths = new String[ttys.length];
            for (int i = 0; i < ttys.length; ++i) {
                devPaths[i] = ttys[i].getAbsolutePath();
            }
        }

        DeviceInfo[] ret = new DeviceInfo[devPaths.length + (Config.ENABLE_TEST_DEVICE ? 1 : 0)];
        for (int i = 0; i < devPaths.length; ++i) {
            ret[i] = new DeviceInfo(devPaths[i]);
        }

        if (Config.ENABLE_TEST_DEVICE) {
            ret[ret.length - 1] = new DeviceInfo("", "Test device");
        }

        return ret;
    }
}
