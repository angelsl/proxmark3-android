#include <stddef.h>

#ifndef PROXMARK3_GLUE_H
#define PROXMARK3_GLUE_H
size_t pm3glue_usbcmd_size(void);
void pm3glue_recv_cmd(void *);
#endif // PROXMARK3_GLUE_H
