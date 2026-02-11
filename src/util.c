#include "compiler.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *current_input;
static const char *current_filename;

void error_at(char *loc, const char *fmt, ...) {
    va_list ap;
    int pos;
    int line = 1;
    int col = 1;
    char *line_start = loc;
    char *line_end = loc;

    while (current_input < line_start && line_start[-1] != '\n') line_start--;
    while (*line_end && *line_end != '\n') line_end++;
    pos = loc - line_start;
    {
        char *p = current_input;
        while (p && p < loc) {
            if (*p == '\n') { line++; col = 1; }
            else col++;
            p++;
        }
    }
    if (current_filename) {
        fprintf(stderr, "%s:%d:%d:\n", current_filename, line, col);
    } else {
        fprintf(stderr, "<input>:%d:%d:\n", line, col);
    }
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

void *xcalloc(size_t nmemb, size_t size) {
    void *p = calloc(nmemb, size);
    if (!p) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    return p;
}

char *strndup2(const char *p, int len) {
    char *s = xcalloc(1, len + 1);
    memcpy(s, p, len);
    s[len] = '\0';
    return s;
}

int startswith(const char *p, const char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

void set_current_input(char *input) {
    current_input = input;
}

void set_current_filename(const char *name) {
    current_filename = name;
}
