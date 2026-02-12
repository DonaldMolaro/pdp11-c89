int main(void) {
    int i;
    int sum;
    sum = 0;
    for (i = 0; i < 5; i = i + 1) {
        switch (i) {
            case 0: sum = sum + 1; break;
            case 2: sum = sum + 2; break;
            default: sum = sum + 3; break;
        }
    }
    return sum;
}
