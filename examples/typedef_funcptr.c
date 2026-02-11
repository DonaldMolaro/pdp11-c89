typedef int (*binop)(int a, int b);

typedef int (*unary)(int x);

int putchar(int c);
int puts(char *s);

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int neg(int x) { return -x; }

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
    unary u;
    int a;
    int b;
    int sum;
    int diff;
    int negv;

    a = 12;
    b = 5;

    ops[0] = add;
    ops[1] = sub;
    u = neg;

    sum = add(a, b);
    diff = sub(a, b);
    negv = neg(b);

    puts("typedef_funcptr: add");
    print_int(sum);
    putchar('\n');

    puts("typedef_funcptr: sub");
    print_int(diff);
    putchar('\n');

    puts("typedef_funcptr: neg");
    print_int(negv);
    putchar('\n');

    return 0;
}
