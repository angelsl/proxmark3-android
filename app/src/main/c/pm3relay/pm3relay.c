#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("usage: %s <in fd> <out fd>", argv[0]);
        return -1;
    }
    char *infdstr = argv[1];
    char *outfdstr = argv[2];
}
