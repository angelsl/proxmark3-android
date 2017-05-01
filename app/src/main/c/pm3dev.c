#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pm3dev.h"
#include "pm3uart.h"
#include "pm3util.h"
#include "pm3relayd.h"

enum { DEVTYPE_INVALID, DEVTYPE_DIRECT, DEVTYPE_RELAYED, DEVTYPE_TEST }
        pm3dev_type = DEVTYPE_INVALID;
int pm3dev_fd = -1;
int pm3dev_relayd_outfd = -1;
int pm3dev_relayd_infd = -1;
pid_t pm3dev_relayd_pid = -1;
char *pm3dev_relayd_path = NULL;
int *pm3dev_thread_quit = NULL;

int pm3dev_relay_run(const char *devpath) {
    if (!pm3dev_relayd_path) {
        fprintf(stderr, "pm3dev: relayd_path unset");
        return -1;
    }

    int outp[2], inp[2];
    if (pipe(outp) == -1) {
        perror("pm3dev: pipe(outp)");
        return -1;
    }
    if (pipe(inp) == -1) {
        perror("pm3dev: pipe(inp)");
        close(outp[0]);
        close(outp[1]);
        return -1;
    }

    char fdarg[2][16];
    if (snprintf(fdarg[0], 16, "%d", outp[0]) > 16 || snprintf(fdarg[1], 16, "%d", inp[1])) {
        fprintf(stderr, "pm3dev: warn: super-long fd?");
    }

    const char *const argv[] = {"root", pm3dev_relayd_path, fdarg[0], fdarg[1], devpath, 0};

    pid_t pid = vfork();
    if (pid == 0) {
        execvp("su", (char *const *) argv);
        _exit(-1);
    } else if (pid > 0) {
        pm3dev_relayd_pid = pid;
        pm3dev_relayd_outfd = outp[1];
        pm3dev_relayd_infd = inp[0];
    } else {
        perror("pm3dev: vfork");
        close(outp[0]);
        close(outp[1]);
        close(inp[0]);
        close(inp[1]);
        return -1;
    }

    return 0;
}

void pm3dev_relay_shutdown(void) {
    enum pm3relayd_cmd cmd = RELAYDCMD_EXIT;
    if (pm3util_write(pm3dev_relayd_outfd, &cmd, sizeof(cmd)) == -1) {
        perror("pm3dev: write(outfd) for RELAYDCMD_EXIT");
    }
}

int pm3dev_relay_send(const uint8_t *bytes, const size_t size) {
    enum pm3relayd_cmd cmd = RELAYDCMD_SEND;
    if (pm3util_write(pm3dev_relayd_outfd, &cmd, sizeof(cmd)) == -1) {
        perror("pm3dev: write(outfd) for RELAYDCMD_SEND");
        return -1;
    }
    if (pm3util_write(pm3dev_relayd_outfd, &size, sizeof(size)) == -1) {
        perror("pm3dev: write(outfd) for RELAYDCMD_SEND size");
        return -1;
    }
    if (pm3util_write(pm3dev_relayd_outfd, bytes, size) == -1) {
        perror("pm3dev: write(outfd) for RELAYDCMD_SEND data");
        return -1;
    }
    return 0;
}

int pm3dev_change(const char *newpath) {
    switch (pm3dev_type) {
        case DEVTYPE_DIRECT:
            close(pm3dev_fd);
            pm3dev_fd = -1;
            break;
        case DEVTYPE_RELAYED:
            // TODO
            break;
        case DEVTYPE_TEST:
        case DEVTYPE_INVALID:
            break;
    }

    if (*newpath) {
        int newfd = pm3uart_open(newpath);
        if (newfd == -1) {
            return -1;
        } else {
            pm3dev_fd = newfd;
            pm3dev_type = DEVTYPE_DIRECT;
            return 0;
        }
    } else {
        pm3dev_type = DEVTYPE_TEST;
        return 0;
    }
}

int pm3dev_write(const uint8_t *bytes, const size_t size) {
    switch (pm3dev_type) {
        case DEVTYPE_TEST: {
            printf("USB write: ");
            for (size_t i = 0; i < size; i++) {
                printf("%02X", bytes[i]);
            }
            printf("\n");
            break;
        }
        case DEVTYPE_INVALID:
            fprintf(stderr, "pm3dev_write: pm3dev_type is DEVTYPE_INVALID");
            return -1;
        case DEVTYPE_DIRECT:
            if (pm3util_write(pm3dev_fd, bytes, size) == -1) {
                perror("pm3dev: write(pm3dev_fd)");
                return -1;
            }
        case DEVTYPE_RELAYED:
            return pm3dev_relay_send(bytes, size);
    }

    return 0;
}
