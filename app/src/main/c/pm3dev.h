#include <stdint.h>
#include <termios.h>

#ifndef PROXMARK3_PM3DEV_H
#define PROXMARK3_PM3DEV_H
extern char *pm3dev_relayd_path;
extern int pm3dev_relayd_child_stdoutfd;

int pm3dev_change(const char *);
int pm3dev_write(const uint8_t *, size_t);
int pm3dev_relay_thread(void);
#endif // PROXMARK3_PM3DEV_H
