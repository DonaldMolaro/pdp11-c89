#ifndef PDP11_C89_RUNTIME_H
#define PDP11_C89_RUNTIME_H

void runtime_emit_startup(void);
void runtime_emit_data(void);
void runtime_emit_functions(void);
void runtime_emit_stdio(void);
void runtime_emit_format(void);
void runtime_emit_fileio(void);
void runtime_emit_math(void);

#endif
