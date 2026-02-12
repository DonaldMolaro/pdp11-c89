typedef struct Pair {
    int a;
    int b;
} Pair;

static Pair make_pair(int a, int b) {
    Pair p;
    p.a = a;
    p.b = b;
    return p;
}

int main(void) {
    Pair p;
    p = make_pair(1, 2);
    return p.a + p.b;
}
