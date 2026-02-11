#include "emitter.h"

#include <stdarg.h>
#include <stdio.h>

static FILE *out;

void emit_init(FILE *fp) {
    out = fp;
}

void emitln(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);
    fprintf(out, "\n");
}
