#include "runtime.h"

void runtime_emit_functions(void) {
    runtime_emit_stdio();
    runtime_emit_format();
    runtime_emit_fileio();
    runtime_emit_math();
    runtime_emit_string();
}
