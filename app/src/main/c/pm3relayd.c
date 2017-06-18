#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "pm3relayd.h"
#include "pm3uart.h"
#include "pm3util.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "pm3relayd: usage: %s <device path>\n", argv[0]);
        return -1;
    }

    int devfd = pm3uart_open(argv[1]);
    if (devfd < 0) {
        fprintf(stderr, "pm3relayd: failed to open device\n");
        return -1;
    }

    int epollfd = epoll_create(2);
    if (epollfd < 0) {
        perror("pm3relayd: epoll");
        return -1;
    }

    struct epoll_event events[2] = {{.events = EPOLLIN, .data.u32 = 0}, {.events = EPOLLIN, .data.u32 = 1}};
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, events) == -1) {
        perror("pm3relayd: epoll_ctl (stdin)");
        return -1;
    }

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, devfd, events + 1) == -1) {
        perror("pm3relayd: epoll_ctl (devfd)");
        fprintf(stderr, "pm3relayd: devfd = %d\n", devfd);
        return -1;
    }

    uint8_t *buf = malloc(4096);
    if (!buf) {
        fprintf(stderr, "pm3relayd: failed to allocate buffer\n");
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
                    if (pm3util_read(STDIN_FILENO, &cmd, sizeof(cmd)) == -1) {
                        perror("pm3relayd: read(infd) 1");
                        return -1;
                    }
                    switch (cmd) {
                        case RELAYDCMD_EXIT:
                            return 0;
                        case RELAYDCMD_SEND: {
                            size_t sz = 0;
                            if (pm3util_read(STDIN_FILENO, &sz, sizeof(sz)) == -1) {
                                perror("pm3relayd: read(infd) 2");
                                return -1;
                            }
                            if (sz > 4096) {
                                buf = realloc(buf, sz);
                                if(!buf) {
                                    fprintf(stderr, "pm3relayd: failed to reallocate buffer\n");
                                    return -1;
                                }
                            }
                            if (pm3util_read(STDIN_FILENO, buf, sz) == -1) {
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
                            fprintf(stderr, "pm3relayd: warn: RELAYDCMD_INVALID received\n");
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
                        if (pm3util_write(STDOUT_FILENO, buf, (size_t) rd) == -1) {
                            perror("pm3relayd: write(outfd)");
                        }
                    }
                    break;
                }
                default:
                    fprintf(stderr, "pm3relayd: strange data from epoll\n");
                    break;
            }
        }
    }
}
