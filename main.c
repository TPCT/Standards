#include <stdio.h>
#include "standards.h"
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    char *x = "data";
    for (int i = 0; i < 4; i++) {
        putChar(*(x + i));
    }
    putChar('\n');
    return 0;
}
