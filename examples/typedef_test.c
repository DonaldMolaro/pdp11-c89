typedef int myint;

typedef struct Pair {
    int a;
    int b;
} Pair;

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
    myint x;
    Pair p;
    int sum;
    x = 7;
    p.a = 1;
    p.b = 2;
    sum = x + p.a + p.b;
    puts("typedef_test: sum=");
    print_int(sum);
    putchar('\n');
    return 0;
}
