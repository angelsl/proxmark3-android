#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "pm3uart.h"

// adapted from original proxmark3 uart.c

int pm3uart_open(const char* devpath) {
    int fd = open(devpath, O_RDWR | O_NOCTTY);
    if(fd == -1) {
        perror("pm3uart_open: open");
        return -1;
    }

    struct flock fl = {
            .l_type = F_WRLCK,
            .l_whence = SEEK_SET,
            .l_pid = getpid()
    };
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("pm3uart_open: fcntl(F_SETLK)");
        close(fd);
        return -1;
    }

    struct termios tinfo;
    if(tcgetattr(fd, &tinfo) == -1) {
        perror("pm3uart_open: tcgetattr");
        close(fd);
        return -1;
    }

    tinfo.c_cflag = CS8 | CLOCAL | CREAD;
    tinfo.c_iflag = IGNPAR;
    tinfo.c_oflag = 0;
    tinfo.c_lflag = 0;
    tinfo.c_cc[VMIN] = 0;
    tinfo.c_cc[VTIME] = 0;

    if(tcsetattr(fd, TCSANOW, &tinfo) == -1) {
        perror("pm3uart_open: tcsetattr");
        close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);

    if (pm3uart_set_speed(fd, B460800) == -1 && pm3uart_set_speed(fd, B115200) == -1) {
        printf("pm3uart_open: failed to set speed");
        close(fd);
        return -1;
    }

    return fd;
}

int pm3uart_set_speed(int fd, const speed_t speed) {
    struct termios ti;
    if (tcgetattr(fd, &ti) == -1) {
        perror("pm3uart_set_speed: tcgetattr");
        return -1;
    }
    cfsetispeed(&ti, speed);
    cfsetospeed(&ti, speed);
    if (tcsetattr(fd, TCSANOW, &ti) == -1) {
        perror("pm3uart_set_speed: tcgetattr");
        return -1;
    }

    return 0;
}
