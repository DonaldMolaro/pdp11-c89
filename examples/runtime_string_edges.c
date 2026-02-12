int putchar(int c);
int puts(char *s);

unsigned int strlen(char *s);
int strcmp(char *a, char *b);
void *memcpy(void *dst, void *src, unsigned int n);
void *memset(void *dst, int c, unsigned int n);
int memcmp(void *a, void *b, unsigned int n);
char *strcpy(char *dst, char *src);

void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    char a[8];
    char b[8];
    int cmp1;
    int cmp2;
    int cmp3;

    memset(a, 0, 8);
    memset(b, 0, 8);

    strcpy(a, "ab");
    strcpy(b, "ac");

    cmp1 = strcmp(a, b);
    cmp2 = memcmp(a, b, 1);
    cmp3 = memcmp(a, b, 2);

    puts("runtime_string_edges: len=");
    print_int((int)strlen(a));
    putchar('\n');

    puts("runtime_string_edges: strcmp=");
    print_int(cmp1);
    putchar('\n');

    puts("runtime_string_edges: memcmp1=");
    print_int(cmp2);
    putchar('\n');

    puts("runtime_string_edges: memcmp2=");
    print_int(cmp3);
    putchar('\n');

    return 0;
}
