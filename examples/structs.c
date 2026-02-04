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

struct Point {
    int x;
    char y;
};

union U {
    int x;
    char c;
};

enum Mode {
    MODE_A = 1,
    MODE_B = 3
};

int main(void) {
    struct Point p;
    union U u;
    int v;

    p.x = 5;
    p.y = 2;
    u.x = 0;
    u.c = 7;

    v = p.x + p.y + u.x;
    if (MODE_A < MODE_B)
        v = v + 1;

    puts("structs: p.x=");
    print_int(p.x);
    puts(" structs: p.y=");
    print_int(p.y);
    puts(" structs: u.x=");
    print_int(u.x);
    puts(" structs: v=");
    print_int(v);
    putchar('\n');
    return 0;
}
