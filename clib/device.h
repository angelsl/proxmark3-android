#include <stdint.h>

extern const char *device_path;

void device_change(const char *newPath);
int device_write(const uint8_t *bytes, const int size);
