package angelsl.androidapp.proxmark3.activities;

import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import angelsl.androidapp.proxmark3.R;
import angelsl.androidapp.proxmark3.interop.Device;

import java.util.ArrayList;
import java.util.Arrays;

public class SelectDeviceActivity extends AppCompatActivity {
    private ListView _listView;
    private ArrayAdapter<Device> _listViewAdapter;
    private Toolbar _toolbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select_device);
        _listView = (ListView) findViewById(R.id.select_device_list);
        _toolbar = (Toolbar) findViewById(R.id.toolbar);

        _listViewAdapter = new ArrayAdapter<>(this, R.layout.select_device_list_item, new ArrayList<>(Arrays.asList(Device.findDevices())));
        _listView.setAdapter(_listViewAdapter);
        _listView.setOnItemClickListener(new DeviceSelectedListener());

        setSupportActionBar(_toolbar);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.select_device_toolbar, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()) {
            case R.id.refresh:
                _listViewAdapter.clear();
                _listViewAdapter.addAll(Device.findDevices());
                return true;
        }

        return false;
    }

    private class DeviceSelectedListener implements AdapterView.OnItemClickListener {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            new AlertDialog.Builder(SelectDeviceActivity.this)
                    .setTitle("Device selected")
                    .setMessage(_listViewAdapter.getItem(position).getDisplayName())
                    .show();
            // TODO open actual activity
        }
    }
}
