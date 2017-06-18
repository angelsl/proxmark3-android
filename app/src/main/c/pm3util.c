#include "pm3util.h"

#include <unistd.h>
#include <stdint.h>

int pm3util_read(int fd, void *rbuf, size_t sz) {
    uint8_t *buf = rbuf;
    size_t rmn = sz;
    while (rmn) {
        ssize_t res = read(fd, buf, rmn);
        if (res >= 0) {
            rmn -= res;
            buf += res;
        } else {
            return -1;
        }
    }
    return 0;
}

int pm3util_write(int fd, const void *rbuf, size_t sz) {
    const uint8_t *buf = rbuf;
    size_t rmn = sz;
    while (rmn) {
        ssize_t res = write(fd, buf, rmn);
        if (res >= 0) {
            rmn -= res;
            buf += res;
        } else {
            return -1;
        }
    }
    return 0;
}
