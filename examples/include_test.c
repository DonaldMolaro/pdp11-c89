#include "inc.h"

int putchar(int c);
int puts(char *s);

void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    int v;
    v = X + Y;
    puts("include_test: sum=");
    print_int(v);
    putchar('\n');
    return 0;
}
