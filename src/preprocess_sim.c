#include "compiler.h"

#include <stddef.h>
#include <stdlib.h>

/*
 * Simulator bootstrap preprocessor:
 * - reads all input from stdin (ignores path)
 * - does not handle #include/#define
 * Expect input to be already concatenated or preprocessed.
 */
char *preprocess_file(const char *path) {
    size_t cap = 4096;
    size_t len = 0;
    char *buf = xcalloc(1, cap);
    int c;
    int bol = 1;
    int skipping = 0;

    (void)path;

    while ((c = getchar()) != -1) {
        if (bol && c == '#') {
            skipping = 1;
        }

        if (c == '\n') {
            if (len + 2 >= cap) {
                cap = cap * 2 + 64;
                buf = realloc(buf, cap);
            }
            buf[len++] = '\n';
            bol = 1;
            skipping = 0;
            continue;
        }

        bol = 0;
        if (skipping) continue;

        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = (char)c;
    }

    if (len == 0 || buf[len - 1] != '\n') {
        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = '\n';
    }
    buf[len] = '\0';
    return buf;
}
