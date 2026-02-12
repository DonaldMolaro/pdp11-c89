typedef int (*binop)(int a, int b);

static int add(int a, int b) { return a + b; }

int main(void) {
    binop op;
    op = add;
    return op(2, 3);
}
