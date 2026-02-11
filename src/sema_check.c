#include "sema_internal.h"

static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

void sema_assign_lvar_offsets(Obj *fn) {
    int offset = 0;
    Obj *var;

    fn->has_sret = (fn->ty && (fn->ty->return_ty->kind == TY_STRUCT || fn->ty->return_ty->kind == TY_UNION));

    for (var = fn->locals; var; var = var->next) {
        if (var->is_param) continue;
        offset += var->ty->size;
        offset = align_to(offset, var->ty->align);
        var->offset = -offset;
    }

    fn->stack_size = align_to(offset, 2);

    {
        int idx = 0;
        Obj *p;
        for (p = fn->params; p; p = p->next) {
            p->offset = 4 + (fn->has_sret ? 2 : 0) + idx * 2;
            idx++;
        }
    }
}
