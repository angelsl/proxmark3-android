#include <sys/types.h>
#include <stddef.h>

#ifndef PROXMARK3_PM3UTIL_H
#define PROXMARK3_PM3UTIL_H
int pm3util_read(int fd, void *rbuf, size_t sz);
int pm3util_write(int fd, const void *rbuf, size_t sz);
#endif //PROXMARK3_PM3UTIL_H
