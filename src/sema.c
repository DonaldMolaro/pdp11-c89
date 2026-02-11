#include "sema_internal.h"

void sema(Obj *prog) {
    Obj *fn;
    for (fn = prog; fn; fn = fn->next) {
        if (!fn->is_function) continue;
        sema_assign_lvar_offsets(fn);
        sema_add_type(fn->body);
        sema_fold_constants(fn->body);
    }
}
