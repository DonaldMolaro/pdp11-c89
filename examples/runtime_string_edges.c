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
    char n1[4];
    char n2[4];
    char big1[16];
    char big2[16];
    int cmp1;
    int cmp2;
    int cmp3;
    int ncmp;
    int bcmp;

    memset(a, 0, 8);
    memset(b, 0, 8);
    memset(n1, 0, 4);
    memset(n2, 0, 4);
    memset(big1, 0, 16);
    memset(big2, 0, 16);

    strcpy(a, "ab");
    strcpy(b, "ac");

    cmp1 = strcmp(a, b);
    cmp2 = memcmp(a, b, 1);
    cmp3 = memcmp(a, b, 2);

    n1[0] = 'a';
    n1[1] = 0;
    n1[2] = 'b';
    n2[0] = 'a';
    n2[1] = 0;
    n2[2] = 'c';
    ncmp = memcmp(n1, n2, 3);

    memset(big1, 'x', 16);
    memcpy(big2, big1, 16);
    big2[15] = 'y';
    bcmp = memcmp(big1, big2, 16);

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

    puts("runtime_string_edges: memcmp_nul=");
    print_int(ncmp);
    putchar('\n');

    puts("runtime_string_edges: memcmp_big=");
    print_int(bcmp);
    putchar('\n');

    return 0;
}
