#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#include "device.h"

const char *device_path = NULL;

void device_change(const char *newPath) {
    const char *old_device_path = device_path;
    char *new_device_path = malloc(strlen(newPath) + 1);
    device_path = strcpy(new_device_path, newPath);
    if (old_device_path) {
        free(old_device_path);
    }
}

int device_write(const uint8_t *bytes, const int size) {
    if (!device_path) {
        return -1;
    } else if (!*device_path) {
        printf("USB write: ");
        for (int i = 0; i < size; i++) {
            printf("%02X", bytes[i]);
        }
        printf("\n");
        return 0;
    } else {
        // actual device, write to uart
        // TODO
        return 0;
    }
}
