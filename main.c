#include "standards.h"
#include <unistd.h>

int main(void) {
    FILE *fp = Fopen("/mnt/sda4/CODEWARS/Standards/testing.txt", "r+");
    putC(fp, 'x');
    fflush(fp);
    int data;
    do {
        data = getC(fp);
        if (data == EOF || data == 0)
            break;
        write(1, &data, 1);
    } while (data != EOF || data == 0);
    write(1, "\n", 1);
    return 0;
}
