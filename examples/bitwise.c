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
    int a;
    int b;
    int c;

    a = 6;  /* 0110 */
    b = 3;  /* 0011 */

    c = (a & b) + (a | b) + (a ^ b);
    c = c + (~a);
    c = c + (a << 1);
    c = c + (a >> 1);

    puts("bitwise: result=");
    print_int(c);
    putchar('\n');
    return 0;
}
