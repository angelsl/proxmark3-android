#include <stdint.h>

#ifndef PROXMARK3_PM3DEV_H
#define PROXMARK3_PM3DEV_H

extern const char *device_path;

void pm3dev_change(const char *newPath);
int pm3dev_write(const uint8_t *bytes, const int size);
#endif // PROXMARK3_PM3DEV_H
