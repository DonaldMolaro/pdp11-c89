int putchar(int c);
int puts(char *s);

unsigned int strlen(char *s);
int strcmp(char *a, char *b);
void *memcpy(void *dst, void *src, unsigned int n);
void *memset(void *dst, int c, unsigned int n);

void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    char a[16];
    char b[16];
    unsigned int len;
    int cmp;

    memset(a, 0, 16);
    memset(b, 0, 16);

    a[0] = 'h';
    a[1] = 'i';
    a[2] = 0;

    memcpy(b, a, 3);

    len = strlen(b);
    cmp = strcmp(a, b);

    puts("runtime_string: b=");
    puts(b);

    puts("runtime_string: len=");
    print_int((int)len);
    putchar('\n');

    puts("runtime_string: cmp=");
    print_int(cmp);
    putchar('\n');

    return 0;
}
