#ifndef PDP11_C89_CODEGEN_INTERNAL_H
#define PDP11_C89_CODEGEN_INTERNAL_H

#include "compiler.h"

typedef struct CodegenContext {
    int labelseq;
    char *current_fn;
    Obj *current_fn_obj;
    int break_labels[32];
    int continue_labels[32];
    int break_depth;
    int continue_depth;
} CodegenContext;

void gen_stmt(CodegenContext *ctx, Node *node);

#endif
