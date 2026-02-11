#ifndef PDP11_C89_RUNTIME_HELPERS_H
#define PDP11_C89_RUNTIME_HELPERS_H

void emit_data_load(const char *label, const char *reg, const char *tmp);
void emit_data_load_byte(const char *label, const char *reg, const char *tmp);
void emit_data_store(const char *label, const char *reg, const char *tmp);
void emit_data_clr(const char *label, const char *tmp);

#endif
