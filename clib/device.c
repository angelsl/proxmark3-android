#include "device.h"

const char *device_path = NULL;

void device_change(const char *newPath) {
    device_path = newPath;
}

int device_write(const uint8_t *bytes, const int size) {
    if (!device_path || !*device_path) {
        // test device
        // TODO print bytes
    } else {
        // actual device, write to uart
        // TODO
    }
}
