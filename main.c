#include "standards.h"

int main(void) {
    SFILE *fp = Fopen("testing.txt", "w+");
    putString(fp, "hello world i love c programming");
    Fseek(fp, -3, 2);
    putString(fp, " hello world");
    Fclose(fp);
    return 0;
}
