#define A 1
#define B A

int main(void) {
#if B
    return 1;
#else
    return 0;
#endif
}
