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
import android.view.Menu;
import android.view.MenuItem;

import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import angelsl.androidapp.proxmark3.R;
import angelsl.androidapp.proxmark3.interop.DeviceInfo;
import angelsl.androidapp.proxmark3.interop.Proxmark3;

public class SessionActivity extends AppCompatActivity implements View.OnClickListener, Proxmark3.OutputHandler {
    private Toolbar _toolbar;
    private EditText _cmdText;
    private ImageButton _goButton;
    private RecyclerView _logView;
    private SessionLogAdapter _logViewAdapter;

    private static final int NEW_OUTPUT = 0xeef8a346;
    private Handler _handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_session);

        _toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(_toolbar);

        _goButton = (ImageButton) findViewById(R.id.session_go);
        _goButton.setOnClickListener(this);

        _logView = (RecyclerView) findViewById(R.id.session_view);
        _logViewAdapter = new SessionLogAdapter(this);
        LinearLayoutManager llm = new LinearLayoutManager(this);
        llm.setOrientation(LinearLayoutManager.VERTICAL);
        llm.setStackFromEnd(true);
        _logView.setAdapter(_logViewAdapter);
        _logView.setLayoutManager(llm);

        _cmdText = (EditText) findViewById(R.id.session_cmd);

        _handler = new Handler(Looper.getMainLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case NEW_OUTPUT:
                        String out = (String) msg.obj;
                        appendLog(new SessionLogEntry(SessionLogEntry.Type.OUTPUT, out));
                        break;
                    default:
                        super.handleMessage(msg);
                        break;
                }
            }
        };
        Proxmark3.setOutputHandler(this);
    }

    private void changeDevice(String newPath) {
        new AlertDialog.Builder(SessionActivity.this)
                .setTitle("Device selected")
                .setMessage(newPath)
                .show();
    }

    private void showChangeDeviceDialog() {
        final DeviceInfo[] devices = DeviceInfo.findDevices();
        String[] names = new String[devices.length];
        for (int i = 0; i < names.length; ++i) {
            names[i] = devices[i].toString();
        }
        new AlertDialog.Builder(this)
                .setTitle(R.string.select_device)
                .setItems(names, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        changeDevice(devices[which].getPath());
                    }
                })
                .show();
    }

    private void acceptCommand() {
        String cmd = _cmdText.getText().toString();
        _cmdText.setText("");
        appendLog(new SessionLogEntry(SessionLogEntry.Type.COMMAND, cmd));
        /* int ret = */ Proxmark3.execCommand(cmd);
        // TODO handle command return code
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
        _handler.obtainMessage(NEW_OUTPUT, output).sendToTarget();
    }
}
