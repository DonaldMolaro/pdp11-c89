#ifndef PDP11_C89_EMITTER_H
#define PDP11_C89_EMITTER_H

#include <stdio.h>

void emit_init(FILE *fp);
void emitln(const char *fmt, ...);
void emit_label(const char *label);
void emit_jmp(const char *label);

#endif
