#ifndef PDP11_C89_EMITTER_H
#define PDP11_C89_EMITTER_H

#include <stdio.h>

void emit_init(FILE *fp);
void emitln(const char *fmt, ...);

#endif
