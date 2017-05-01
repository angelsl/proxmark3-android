#include <termios.h>

#ifndef PROXMARK3_PM3UART_H
#define PROXMARK3_PM3UART_H
int pm3uart_open(const char *);
int pm3uart_set_speed(int, const speed_t);
#endif // PROXMARK3_PM3UART_H
