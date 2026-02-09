#include <stdio.h>

int putchar(int c);
int puts(char *s);

static void print_int(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) print_int(x / 10);
    putchar('0' + (x % 10));
}

int main(void) {
    FILE *fp;
    char buf[32];
    int n;
    int i;

    fp = fopen("./pdp11_file_io.txt", "w");
    if (!fp) {
        puts("file_io: open(write) failed\n");
        return 1;
    }

    n = (int)fwrite("Hello from PDP-11!\n", 1, 20, fp);
    puts("file_io: wrote bytes=");
    print_int(n);
    putchar('\n');
    fclose(fp);

    fp = fopen("./pdp11_file_io.txt", "r");
    if (!fp) {
        puts("file_io: open(read) failed\n");
        return 1;
    }

    n = (int)fread(buf, 1, 20, fp);
    buf[n] = '\0';
    puts("file_io: read bytes=");
    print_int(n);
    putchar('\n');
    puts("file_io: contents=\n");
    puts(buf);
    putchar('\n');
    fclose(fp);

    return 0;
}
