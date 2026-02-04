struct Pair { int a; int b; };

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
    struct Pair p;
    p.a = 1;
    p.b = 2;
    puts("struct_min: p.a=");
    print_int(p.a);
    puts(" struct_min: p.b=");
    print_int(p.b);
    putchar('\n');
    return 0;
}
