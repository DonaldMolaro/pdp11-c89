typedef struct Pair {
    int a;
    int b;
} Pair;

typedef Pair (*pairfn)(int a, int b);

int putchar(int c);
int puts(char *s);

static Pair make_pair(int a, int b) {
    Pair p;
    p.a = a;
    p.b = b;
    return p;
}

void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    pairfn fn;
    Pair out;

    fn = make_pair;
    out = fn(3, 4);

    puts("indirect_sret: a=");
    print_int(out.a);
    putchar('\n');

    puts("indirect_sret: b=");
    print_int(out.b);
    putchar('\n');

    return 0;
}
