#include <stdio.h>

static void print_uint(unsigned int v) {
    char buf[6];
    int i = 0;
    if (v == 0) {
        putchar('0');
        return;
    }
    while (v > 0 && i < (int)sizeof(buf)) {
        buf[i++] = (char)('0' + (v % 10));
        v = v / 10;
    }
    while (i-- > 0) {
        putchar(buf[i]);
    }
}

int main(void) {
    FILE *fp;
    char buf[32];
    int ch;
    const char *s = "line1\nline2\n";

    fp = fopen("pdp11_file_io.txt", "w");
    if (!fp) {
        puts("stdio_extra: fopen write failed");
        return 1;
    }
    fwrite(s, 1, 12, fp);
    fclose(fp);

    fp = fopen("pdp11_file_io.txt", "r");
    if (!fp) {
        puts("stdio_extra: fopen read failed");
        return 1;
    }
    puts("stdio_extra: fp=");
    print_uint((unsigned int)fp);
    putchar('\n');

    ch = fgetc(fp);
    ungetc(ch, fp);
    ch = getc(fp);
    puts("stdio_extra: ch=");
    print_uint((unsigned int)ch);
    putchar('\n');
    puts("stdio_extra: first char=");
    putchar(ch);
    putchar('\n');

    if (fgets(buf, (int)sizeof(buf), fp)) {
        puts("stdio_extra: line1=");
        puts(buf);
    }
    if (fgets(buf, (int)sizeof(buf), fp)) {
        puts("stdio_extra: line2=");
        puts(buf);
    }
    if (!fgets(buf, (int)sizeof(buf), fp)) {
        puts("stdio_extra: eof=");
        print_uint((unsigned int)feof(fp));
        putchar('\n');
    }

    fclose(fp);
    fflush(stdout);
    return 0;
}
