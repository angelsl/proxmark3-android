#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <in fd> <out fd>", argv[0]);
        return -1;
    }
    char *end;
    int infd = (int) strtol(argv[1], &end, 10);
    if (!infd && end == argv[1]) {
        fprintf(stderr, "invalid in fd");
        return -1;
    }

    int outfd = (int) strtol(argv[2], &end, 10);
    if (!outfd && end == argv[2]) {
        fprintf(stderr, "invalid out fd");
        return -1;
    }
}
