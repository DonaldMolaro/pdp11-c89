#define X 7
#undef X

int main(void) {
#ifdef X
    return 1;
#else
    return 0;
#endif
}
