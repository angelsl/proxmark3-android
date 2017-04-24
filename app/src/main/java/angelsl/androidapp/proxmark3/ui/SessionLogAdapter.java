package angelsl.androidapp.proxmark3.ui;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import angelsl.androidapp.proxmark3.R;

import java.util.ArrayList;

public class SessionLogAdapter extends RecyclerView.Adapter<SessionLogAdapter.ViewHolder> {
    private Context _context;
    private ArrayList<SessionLogEntry> _entries;

    SessionLogAdapter(Context context) {
        _context = context;
        _entries = new ArrayList<>(256);
    }

    void insertEntry(SessionLogEntry e) {
        _entries.add(e);
        notifyItemInserted(_entries.size() - 1);
    }

    @Override
    public SessionLogAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        int vId;
        switch (viewType) {
            case 0:
                vId = R.layout.console_output;
                break;
            case 1:
                vId = R.layout.console_input;
                break;
            default:
                throw new AssertionError("Unreachable code: invalid viewType " + viewType);
        }
        return new ViewHolder(getLayoutInflater().inflate(vId, parent, false));
    }

    @Override
    public void onBindViewHolder(SessionLogAdapter.ViewHolder holder, int position) {
        holder.getTextView().setText(_entries.get(position).getContents());
    }

    @Override
    public int getItemCount() {
        return _entries.size();
    }

    @Override
    public int getItemViewType(int position) {
        switch (_entries.get(position).getType()) {
            case OUTPUT:
                return 0;
            case COMMAND:
                return 1;
        }

        throw new AssertionError("Unreachable code or switch was not exhaustive?");
    }

    private LayoutInflater getLayoutInflater() {
        return (LayoutInflater) _context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        TextView _text;

        ViewHolder(View itemView) {
            super(itemView);
            _text = (TextView) itemView.findViewById(R.id.text);
        }

        TextView getTextView() {
            return _text;
        }
    }
}
