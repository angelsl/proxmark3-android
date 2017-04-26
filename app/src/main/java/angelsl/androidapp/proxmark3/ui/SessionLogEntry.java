package angelsl.androidapp.proxmark3.ui;

public final class SessionLogEntry {
    private final Type _type;
    private final String _contents;

    public SessionLogEntry(Type type, String contents) {
        _type = type;
        _contents = contents;
    }

    public Type getType() {
        return _type;
    }

    public String getContents() {
        return _contents;
    }

    public enum Type {
        OUTPUT,
        COMMAND
    }
}
