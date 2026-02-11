#include <stdio.h>

int g = 123;

static void print_uint(unsigned int v) {
    char buf[6];
    int i = 0;
    if (v == 0) {
        putchar('0');
        return;
    }
    while (v > 0 && i < (int)sizeof(buf)) {
        buf[i++] = (char)('0' + (v % 10));
        v = v / 10;
    }
    while (i-- > 0) {
        putchar(buf[i]);
    }
}

int main(void) {
    int v0;
    int v1;

    pdp11_set_bank(0);
    v0 = g;
    pdp11_set_bank(1);
    v1 = g;

    puts("bank0 g=");
    print_uint((unsigned int)v0);
    putchar('\n');
    puts("bank1 g=");
    print_uint((unsigned int)v1);
    putchar('\n');
    return 0;
}
