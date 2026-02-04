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
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 5; i = i + 1) {
        if (i == 2)
            continue;
        switch (i) {
            case 1:
                sum = sum + 10;
                break;
            case 3:
                sum = sum + 30;
                break;
            default:
                sum = sum + 1;
        }
    }
    puts("switch_for: sum=");
    print_int(sum);
    putchar('\n');
    return 0;
}
