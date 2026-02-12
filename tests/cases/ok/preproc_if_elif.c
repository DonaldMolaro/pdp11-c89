#define A 0
#define B 1

int main(void) {
#if A
    return 11;
#elif defined(B) && B
    return 22;
#else
    return 33;
#endif
}
