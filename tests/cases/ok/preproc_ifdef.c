#define FLAG 1

int main(void) {
#ifdef FLAG
    return 1;
#else
    return 0;
#endif
}
