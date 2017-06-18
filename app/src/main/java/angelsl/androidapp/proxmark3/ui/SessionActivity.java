package angelsl.androidapp.proxmark3.ui;

import android.content.DialogInterface;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;

import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;

import angelsl.androidapp.proxmark3.R;
import angelsl.androidapp.proxmark3.interop.DeviceInfo;
import angelsl.androidapp.proxmark3.interop.Proxmark3;

public class SessionActivity extends AppCompatActivity implements View.OnClickListener, Proxmark3.OutputHandler, TextView.OnEditorActionListener {
    private Toolbar _toolbar;
    private EditText _cmdText;
    private ImageButton _goButton;
    private ProgressBar _goWait;
    private RecyclerView _logView;
    private SessionLogAdapter _logViewAdapter;

    private Handler _handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_session);

        _toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(_toolbar);

        _goButton = (ImageButton) findViewById(R.id.session_go);
        _goButton.setOnClickListener(this);

        _goWait = (ProgressBar) findViewById(R.id.session_go_wait);

        _logView = (RecyclerView) findViewById(R.id.session_view);
        _logViewAdapter = new SessionLogAdapter(this);
        LinearLayoutManager llm = new LinearLayoutManager(this);
        llm.setOrientation(LinearLayoutManager.VERTICAL);
        llm.setStackFromEnd(true);
        _logView.setAdapter(_logViewAdapter);
        _logView.setLayoutManager(llm);

        _cmdText = (EditText) findViewById(R.id.session_cmd);
        _cmdText.setOnEditorActionListener(this);

        _handler = new EventHandler(new WeakReference<>(this));
        Proxmark3.setOutputHandler(this);
        Proxmark3.init(this);
    }

    private void showChangeDeviceDialog() {
        final DeviceInfo[] devices = DeviceInfo.findDevices();

        if (devices.length == 0) {
            new AlertDialog.Builder(this)
                    .setTitle(R.string.select_device)
                    .setMessage(R.string.no_devices)
                    .setPositiveButton(R.string.ok, null)
                    .show();
            return;
        }

        String[] names = new String[devices.length];
        for (int i = 0; i < names.length; ++i) {
            names[i] = devices[i].toString();
        }
        new AlertDialog.Builder(this)
                .setTitle(R.string.select_device)
                .setItems(names, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Proxmark3.changeDevice(devices[which]);
                    }
                })
                .show();
    }

    private void acceptCommand() {
        if (!_goButton.isEnabled()) {
            return;
        }

        final String cmd = _cmdText.getText().toString().trim();
        if (cmd.isEmpty()) {
            return;
        }

        _goButton.setEnabled(false);
        _goButton.setVisibility(View.INVISIBLE);
        _goWait.setVisibility(View.VISIBLE);
        _cmdText.setText("");
        appendLog(new SessionLogEntry(SessionLogEntry.Type.COMMAND, cmd));
        Proxmark3.execCommand(cmd);
    }

    private void appendLog(SessionLogEntry entry) {
        _logViewAdapter.insertEntry(entry);
        _logView.scrollToPosition(_logViewAdapter.getItemCount() - 1);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.session, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.select_device:
                showChangeDeviceDialog();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.session_go:
                acceptCommand();
                break;
        }
    }

    @Override
    public void onOutput(String output) {
        _handler.obtainMessage(EventType.NEW_OUTPUT.ordinal(), output.replaceAll("[\\r\\n]+$", "")).sendToTarget();
    }

    @Override
    public void onCommandCompletion() {
        _handler.obtainMessage(EventType.COMMAND_COMPLETED.ordinal()).sendToTarget();
    }

    @Override
    public void onChangeDevice(DeviceInfo newDevice) {
        _handler.obtainMessage(EventType.DEVICE_CHANGED.ordinal(), newDevice).sendToTarget();
    }

    @Override
    public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
        switch (actionId) {
            case EditorInfo.IME_NULL:
                if (event.getKeyCode() != KeyEvent.KEYCODE_ENTER) {
                    break;
                }
            case EditorInfo.IME_ACTION_SEND:
                acceptCommand();
                return true;
        }

        return false;
    }

    private static class EventHandler extends Handler {
        private final WeakReference<SessionActivity> _wrsa;

        EventHandler(WeakReference<SessionActivity> wrsa) {
            super(Looper.getMainLooper());
            _wrsa = wrsa;
        }

        @Override
        public void handleMessage(Message msg) {
            SessionActivity me = _wrsa.get();

            if (me == null) {
                return;
            }

            switch (EventType.fromOrdinal(msg.what)) {
                case NEW_OUTPUT:
                    me.appendLog(new SessionLogEntry(SessionLogEntry.Type.OUTPUT, (String) msg.obj));
                    break;
                case COMMAND_COMPLETED:
                    me._goButton.setEnabled(true);
                    me._goButton.setVisibility(View.VISIBLE);
                    me._goWait.setVisibility(View.GONE);
                    break;
                case DEVICE_CHANGED:
                    DeviceInfo newDev = (DeviceInfo) msg.obj;
                    me.appendLog(new SessionLogEntry(SessionLogEntry.Type.COMMAND, String.format("(%s %s)", me.getResources().getString(R.string.device_changed_to), newDev)));
                    me._toolbar.setTitle(String.format("%s (%s)", me.getResources().getString(R.string.app_name), newDev));
                    break;
                default:
                    super.handleMessage(msg);
                    break;
            }
        }
    }

    private enum EventType {
        INVALID,
        NEW_OUTPUT,
        COMMAND_COMPLETED,
        DEVICE_CHANGED;

        private static final EventType[] _values = values();

        public static EventType fromOrdinal(int o) {
            if (o < 0 || o >= _values.length) {
                return INVALID;
            }

            return _values[o];
        }
    }
}
