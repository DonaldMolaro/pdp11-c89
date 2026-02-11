#ifndef PDP11_C89_SEMA_INTERNAL_H
#define PDP11_C89_SEMA_INTERNAL_H

#include "compiler.h"

void sema_add_type(Node *node);
void sema_assign_lvar_offsets(Obj *fn);
void sema_fold_constants(Node *node);

#endif
