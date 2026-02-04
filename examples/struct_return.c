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

struct Pair {
    int a;
    int b;
};

struct Pair make(int x, int y) {
    struct Pair p;
    p.a = x;
    p.b = y;
    return p;
}

int main(void) {
    struct Pair q;
    q = make(3, 4);
    puts("struct_return: a=");
    print_int(q.a);
    puts(" struct_return: b=");
    print_int(q.b);
    putchar('\n');
    return 0;
}
