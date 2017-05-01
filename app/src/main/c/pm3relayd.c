#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "pm3relayd.h"
#include "pm3uart.h"
#include "pm3util.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "pm3relayd: usage: %s <in fd> <out fd> <device path>", argv[0]);
        return -1;
    }

    char *end;
    int infd = (int) strtol(argv[1], &end, 10);
    if (!infd && end == argv[1]) {
        fprintf(stderr, "pm3relayd: invalid in fd");
        return -1;
    }
    int outfd = (int) strtol(argv[2], &end, 10);
    if (!outfd && end == argv[2]) {
        fprintf(stderr, "pm3relayd: invalid out fd");
        return -1;
    }

    int devfd = pm3uart_open(argv[3]);
    if (devfd < 0) {
        fprintf(stderr, "pm3relayd: failed to open device");
        return -1;
    }

    int epollfd = epoll_create(2);
    if (epollfd < 0) {
        perror("pm3relayd: epoll");
        return -1;
    }

    struct epoll_event events[2] = {{.events = EPOLLIN, .data.u32 = 0}, {.events = EPOLLIN, .data.u32 = 1}};
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, events) == -1
            || epoll_ctl(epollfd, EPOLL_CTL_ADD, devfd, events + 1) == -1) {
        perror("pm3relayd: epoll_ctl");
        return -1;
    }

    uint8_t *buf = malloc(4096);
    if (!buf) {
        fprintf(stderr, "pm3relayd: failed to allocate buffer");
        return -1;
    }

    while (1) {
        struct epoll_event events[2];
        int nfds = epoll_wait(epollfd, events, 2, -1);
        if (nfds == -1) {
            perror("pm3relayd: epoll_wait");
            return -1;
        }

        for (int i = 0; i < nfds; ++nfds) {
            struct epoll_event *event = events + i;
            switch (event->data.u32) {
                case 0: { // infd
                    enum pm3relayd_cmd cmd = -1;
                    if (pm3util_read(infd, &cmd, sizeof(cmd)) == -1) {
                        perror("pm3relayd: read(infd) 1");
                        return -1;
                    }
                    switch (cmd) {
                        case RELAYDCMD_EXIT:
                            return 0;
                        case RELAYDCMD_SEND: {
                            size_t sz = 0;
                            if (pm3util_read(infd, &sz, sizeof(sz)) == -1) {
                                perror("pm3relayd: read(infd) 2");
                                return -1;
                            }
                            if (sz > 4096) {
                                buf = realloc(buf, sz);
                                if(!buf) {
                                    fprintf(stderr, "pm3relayd: failed to reallocate buffer");
                                    return -1;
                                }
                            }
                            if (pm3util_read(infd, buf, sz) == -1) {
                                perror("pm3relayd: read(infd) 3");
                                return -1;
                            }
                            if (pm3util_write(devfd, buf, sz) == -1) {
                                perror("pm3relayd: write(devfd)");
                                return -1;
                            }
                            break;
                        }
                        case RELAYDCMD_INVALID:
                            fprintf(stderr, "pm3relayd: warn: RELAYDCMD_INVALID received");
                            break;
                    }
                    break;
                }
                case 1: { // devfd
                    ssize_t rd = read(devfd, buf, 4096);
                    if (rd < 0) {
                        perror("pm3relayd: read(devfd)");
                        return -1;
                    } else if (rd > 0) {
                        if (pm3util_write(outfd, buf, (size_t) rd) == -1) {
                            perror("pm3relayd: write(outfd)");
                        }
                    }
                    break;
                }
                default:
                    fprintf(stderr, "pm3relayd: strange data from epoll");
                    break;
            }
        }
    }
}
