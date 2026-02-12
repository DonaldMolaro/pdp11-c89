typedef int (*binop)(int a, int b);

int putchar(int c);
int puts(char *s);

static int add(int a, int b) { return a + b; }
static int mul(int a, int b) { return a * b; }

void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    binop ops[2];
    int a;
    int b;

    a = 6;
    b = 7;

    ops[0] = add;
    ops[1] = mul;

    puts("indirect_call: add");
    print_int(ops[0](a, b));
    putchar('\n');

    puts("indirect_call: mul");
    print_int(ops[1](a, b));
    putchar('\n');

    return 0;
}
