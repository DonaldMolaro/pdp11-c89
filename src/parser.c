#include "compiler.h"

#include <string.h>

#include <stdio.h>
#include <string.h>

static Token *token;

typedef struct VarScope VarScope;
struct VarScope {
    VarScope *next;
    char *name;
    Obj *var;
};

typedef struct TagScope TagScope;
struct TagScope {
    TagScope *next;
    char *name;
    Type *ty;
};

typedef struct TypedefScope TypedefScope;
struct TypedefScope {
    TypedefScope *next;
    char *name;
    Type *ty;
};

typedef struct Scope Scope;
struct Scope {
    Scope *next;
    VarScope *vars;
    TagScope *tags;
    TypedefScope *typedefs;
};

static Scope *scope;
static Obj *locals;
static Obj *globals;
static Obj *current_fn;
static Node *current_switch;
static int loop_depth;
static int decl_is_static;
static int decl_is_extern;
static char *file_prefix;

static Type *basetype(void);
static Type *declarator(Type *ty, char **name, Obj **params);

void set_file_prefix(const char *prefix) {
    file_prefix = (char *)prefix;
}

static char *make_asm_name(const char *name, int is_static) {
    int i;
    int len_name = (int)strlen(name);
    int len_pref = (file_prefix && is_static) ? (int)strlen(file_prefix) : 0;
    char *buf = xcalloc(1, len_pref + len_name + 1);
    for (i = 0; i < len_pref; i++) buf[i] = file_prefix[i];
    for (i = 0; i < len_name; i++) buf[len_pref + i] = name[i];
    buf[len_pref + len_name] = '\0';
    return buf;
}

static void enter_scope(void) {
    Scope *sc = xcalloc(1, sizeof(Scope));
    sc->next = scope;
    scope = sc;
}

static void leave_scope(void) {
    scope = scope->next;
}

static void push_scope(char *name, Obj *var) {
    VarScope *sc = xcalloc(1, sizeof(VarScope));
    sc->name = name;
    sc->var = var;
    sc->next = scope->vars;
    scope->vars = sc;
}

static void push_tag_scope(char *name, Type *ty) {
    TagScope *sc = xcalloc(1, sizeof(TagScope));
    sc->name = name;
    sc->ty = ty;
    sc->next = scope->tags;
    scope->tags = sc;
}

static void push_typedef_scope(char *name, Type *ty) {
    TypedefScope *sc = xcalloc(1, sizeof(TypedefScope));
    sc->name = name;
    sc->ty = ty;
    sc->next = scope->typedefs;
    scope->typedefs = sc;
}

static Type *find_typedef(Token *tok) {
    Scope *sc;
    TypedefScope *td;

    for (sc = scope; sc; sc = sc->next) {
        for (td = sc->typedefs; td; td = td->next) {
            if (strlen(td->name) == (size_t)tok->len &&
                strncmp(td->name, tok->loc, tok->len) == 0) {
                return td->ty;
            }
        }
    }
    return NULL;
}

static Type *find_tag(Token *tok) {
    Scope *sc;
    TagScope *tag;

    for (sc = scope; sc; sc = sc->next) {
        for (tag = sc->tags; tag; tag = tag->next) {
            if (strlen(tag->name) == (size_t)tok->len &&
                strncmp(tag->name, tok->loc, tok->len) == 0) {
                return tag->ty;
            }
        }
    }
    return NULL;
}

static Obj *find_var(Token *tok) {
    Scope *sc;
    VarScope *var;

    for (sc = scope; sc; sc = sc->next) {
        for (var = sc->vars; var; var = var->next) {
            if (strlen(var->name) == (size_t)tok->len &&
                strncmp(var->name, tok->loc, tok->len) == 0) {
                return var->var;
            }
        }
    }
    return NULL;
}

static int consume(int kind) {
    if (token->kind != kind) return 0;
    token = token->next;
    return 1;
}

static void expect(int kind) {
    if (token->kind != kind) error_at(token->loc, "unexpected token");
    token = token->next;
}

static long expect_number(void) {
    if (token->kind != TK_NUM) error_at(token->loc, "expected number");
    long val = token->val;
    token = token->next;
    return val;
}

static char *expect_ident(void) {
    if (token->kind != TK_IDENT) error_at(token->loc, "expected identifier");
    char *name = strndup2(token->loc, token->len);
    token = token->next;
    return name;
}

static Obj *new_var(char *name, Type *ty, int is_local, int is_static, int is_extern) {
    Obj *var = xcalloc(1, sizeof(Obj));
    var->name = name;
    var->ty = ty;
    var->is_local = is_local;
    var->is_static = (!is_local && is_static);
    var->is_extern = (!is_local && is_extern);
    var->asm_name = make_asm_name(name, var->is_static);
    if (is_local) {
        var->next = locals;
        locals = var;
    } else {
        var->next = globals;
        globals = var;
        push_scope(name, var);
    }
    return var;
}

static Obj *new_param(char *name, Type *ty) {
    Obj *var = xcalloc(1, sizeof(Obj));
    var->name = name;
    var->ty = ty;
    var->is_local = 1;
    var->is_param = 1;
    return var;
}

static Obj *new_enum_const(char *name, long val) {
    Obj *var = xcalloc(1, sizeof(Obj));
    var->name = name;
    var->ty = ty_int();
    var->is_enum_const = 1;
    var->enum_val = val;
    push_scope(name, var);
    return var;
}

static int is_typename(void) {
    if (token->kind == TK_INT || token->kind == TK_CHAR_KW || token->kind == TK_VOID ||
        token->kind == TK_STRUCT || token->kind == TK_UNION || token->kind == TK_ENUM ||
        token->kind == TK_TYPEDEF || token->kind == TK_STATIC || token->kind == TK_EXTERN ||
        token->kind == TK_CONST || token->kind == TK_VOLATILE || token->kind == TK_SIGNED ||
        token->kind == TK_UNSIGNED || token->kind == TK_SHORT || token->kind == TK_LONG) {
        return 1;
    }
    if (token->kind == TK_IDENT && find_typedef(token)) return 1;
    return 0;
}

static int is_typename_token(Token *tok) {
    Token *saved = token;
    int ret;
    token = tok;
    ret = is_typename();
    token = saved;
    return ret;
}

static Member *struct_members(void) {
    Member head;
    Member *cur = &head;

    while (!consume('}')) {
        Type *base = NULL;
        if (!is_typename()) error_at(token->loc, "expected type");
        base = basetype();
        for (;;) {
            char *name = NULL;
            Obj *dummy = NULL;
            Type *ty = declarator(base, &name, &dummy);
            Member *mem = xcalloc(1, sizeof(Member));
            mem->name = name;
            mem->ty = ty;
            cur = cur->next = mem;
            if (consume(';')) break;
            expect(',');
        }
    }
    return head.next;
}

static Type *struct_decl(int is_union) {
    Token *tag = NULL;
    Type *ty;

    if (token->kind == TK_IDENT) {
        tag = token;
        token = token->next;
    }

    if (consume('{')) {
        Member *members = struct_members();
        if (tag) {
            Type *existing = find_tag(tag);
            if (existing) {
                if (!existing->is_incomplete) error_at(tag->loc, "redefinition of struct/union");
                ty = struct_type(is_union, existing->tag, members);
                existing->members = ty->members;
                existing->size = ty->size;
                existing->align = ty->align;
                existing->is_incomplete = 0;
                return existing;
            }
        }
        ty = struct_type(is_union, tag ? strndup2(tag->loc, tag->len) : NULL, members);
        if (tag) push_tag_scope(ty->tag, ty);
        return ty;
    }

    if (!tag) error_at(token->loc, "struct/union tag missing");
    ty = find_tag(tag);
    if (!ty) {
        ty = struct_type(is_union, strndup2(tag->loc, tag->len), NULL);
        push_tag_scope(ty->tag, ty);
    }
    return ty;
}

static Type *enum_decl(void) {
    Token *tag = NULL;
    if (token->kind == TK_IDENT) {
        tag = token;
        token = token->next;
    }

    if (consume('{')) {
        long val = 0;
        for (;;) {
            char *name = expect_ident();
            if (consume('=')) {
                val = expect_number();
            }
            new_enum_const(name, val++);
            if (consume('}')) break;
            expect(',');
        }
    }

    if (tag && !find_tag(tag)) {
        Type *ty = struct_type(0, strndup2(tag->loc, tag->len), NULL);
        push_tag_scope(ty->tag, ty);
    }

    return ty_int();
}

static Type *basetype(void) {
    int saw_int = 0;
    int saw_char = 0;
    int saw_void = 0;
    int saw_signed = 0;
    int saw_unsigned = 0;
    int saw_short = 0;
    int saw_long = 0;
    decl_is_static = 0;
    decl_is_extern = 0;

    for (;;) {
        if (consume(TK_TYPEDEF) || consume(TK_CONST) || consume(TK_VOLATILE)) {
            continue;
        }
        if (consume(TK_STATIC)) { decl_is_static = 1; decl_is_extern = 0; continue; }
        if (consume(TK_EXTERN)) { decl_is_extern = 1; decl_is_static = 0; continue; }
        if (consume(TK_SIGNED)) { saw_signed = 1; continue; }
        if (consume(TK_UNSIGNED)) { saw_unsigned = 1; continue; }
        if (consume(TK_SHORT)) { saw_short = 1; continue; }
        if (consume(TK_LONG)) { saw_long = 1; continue; }
        if (consume(TK_INT)) { saw_int = 1; break; }
        if (consume(TK_CHAR_KW)) { saw_char = 1; break; }
        if (consume(TK_VOID)) { saw_void = 1; break; }
        if (consume(TK_STRUCT)) return struct_decl(0);
        if (consume(TK_UNION)) return struct_decl(1);
        if (consume(TK_ENUM)) return enum_decl();
        if (token->kind == TK_IDENT) {
            Type *td = find_typedef(token);
            if (td) {
                token = token->next;
                return td;
            }
        }
        break;
    }

    if (saw_char) return ty_char();
    if (saw_void) return ty_void();
    if (saw_int || saw_signed || saw_unsigned || saw_short || saw_long) return ty_int();

    error_at(token->loc, "unknown type");
    return ty_int();
}

static Obj *param_list(void) {
    Obj head;
    Obj *cur = &head;
    head.next = NULL;

    if (consume(')')) return NULL;
    if (token->kind == TK_VOID && token->next->kind == ')') {
        token = token->next;
        expect(')');
        return NULL;
    }

    for (;;) {
        if (consume(TK_ELLIPSIS)) {
            expect(')');
            break;
        }
        Type *base = basetype();
        char *pname = NULL;
        Obj *dummy = NULL;
        Type *pty = declarator(base, &pname, &dummy);
        Obj *var = new_param(pname, pty);
        cur = cur->next = var;

        if (consume(')')) break;
        expect(',');
    }

    return head.next;
}

static Type *type_suffix(Type *ty, Obj **params) {
    if (consume('[')) {
        int len = (int)expect_number();
        expect(']');
        ty = type_suffix(ty, params);
        return array_of(ty, len);
    }
    if (consume('(')) {
        Obj *plist = param_list();
        *params = plist;
        return func_type(ty, NULL);
    }
    return ty;
}

static Type *declarator(Type *ty, char **name, Obj **params) {
    while (consume('*')) {
        ty = pointer_to(ty);
    }

    *name = expect_ident();
    return type_suffix(ty, params);
}

static Node *stmt(void);
static Node *expr(void);
static long eval_const(Node *node);
static Node *declaration_stmt(void);

static Node *primary(void) {
    if (consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    if (token->kind == TK_NUM) {
        Node *node = new_num(token->val);
        token = token->next;
        return node;
    }

    if (token->kind == TK_CHAR) {
        Node *node = new_num(token->val);
        token = token->next;
        node->ty = ty_char();
        return node;
    }

    if (token->kind == TK_STR) {
        Token *tok = token;
        token = token->next;
        Obj *var;
        char namebuf[32];
        static int str_id = 0;
        sprintf(namebuf, ".L.str%d", str_id++);
        var = new_var(strndup2(namebuf, (int)strlen(namebuf)), array_of(ty_char(), tok->str_len + 1), 0, 0, 0);
        var->init_data = tok->str;
        var->init_len = tok->str_len;
        return new_unary(ND_ADDR, new_var_node(var));
    }

    if (token->kind == TK_IDENT) {
        Obj *var = find_var(token);
        if (!var) error_at(token->loc, "undefined variable");
        token = token->next;
        if (var->is_enum_const) return new_num(var->enum_val);
        return new_var_node(var);
    }

    error_at(token->loc, "expected expression");
    return NULL;
}

static Node *postfix(void) {
    Node *node = primary();

    for (;;) {
        if (consume('(')) {
            Node head;
            Node *cur = &head;
            head.next = NULL;
            while (!consume(')')) {
                if (cur != &head) expect(',');
                cur = cur->next = expr();
            }
            if (!node->var) error_at(token->loc, "invalid function call");
            {
                Node *call = new_unary(ND_FUNCALL, NULL);
                call->var = node->var;
                call->args = head.next;
                node = call;
            }
            continue;
        }

        if (consume('[')) {
            Node *idx = expr();
            expect(']');
            node = new_unary(ND_DEREF, new_binary(ND_ADD, node, idx));
            continue;
        }

        if (consume('.')) {
            Node *mem = new_node(ND_MEMBER);
            mem->lhs = node;
            mem->member_name = expect_ident();
            node = mem;
            continue;
        }

        if (consume(TK_ARROW)) {
            Node *mem = new_node(ND_MEMBER);
            mem->lhs = new_unary(ND_DEREF, node);
            mem->member_name = expect_ident();
            node = mem;
            continue;
        }

        if (consume(TK_INC)) {
            return new_unary(ND_POST_INC, node);
        }

        if (consume(TK_DEC)) {
            return new_unary(ND_POST_DEC, node);
        }

        return node;
    }
}

static Node *unary(void) {
    if (consume(TK_INC)) return new_unary(ND_PRE_INC, unary());
    if (consume(TK_DEC)) return new_unary(ND_PRE_DEC, unary());
    if (token->kind == '(' && is_typename_token(token->next)) {
        token = token->next;
        {
            Type *ty = basetype();
            while (consume('*')) ty = pointer_to(ty);
            expect(')');
            {
                Node *node = new_unary(ND_CAST, unary());
                node->cast_ty = ty;
                return node;
            }
        }
    }
    if (consume('+')) return unary();
    if (consume('-')) return new_unary(ND_NEG, unary());
    if (consume('&')) return new_unary(ND_ADDR, unary());
    if (consume('*')) return new_unary(ND_DEREF, unary());
    if (consume('!')) return new_unary(ND_NOT, unary());
    if (consume('~')) return new_unary(ND_BITNOT, unary());
    if (consume(TK_SIZEOF)) {
        if (consume('(')) {
            if (is_typename()) {
                Type *ty = basetype();
                while (consume('*')) ty = pointer_to(ty);
                if (consume('[')) {
                    int len = (int)expect_number();
                    expect(']');
                    ty = array_of(ty, len);
                }
                expect(')');
                return new_num(ty->size);
            } else {
                Node *node = new_unary(ND_SIZEOF, expr());
                expect(')');
                return node;
            }
        }
        return new_unary(ND_SIZEOF, unary());
    }
    return postfix();
}

static Node *mul(void) {
    Node *node = unary();

    for (;;) {
        if (consume('*')) node = new_binary(ND_MUL, node, unary());
        else if (consume('/')) node = new_binary(ND_DIV, node, unary());
        else if (consume('%')) node = new_binary(ND_MOD, node, unary());
        else return node;
    }
}

static Node *add(void) {
    Node *node = mul();

    for (;;) {
        if (consume('+')) node = new_binary(ND_ADD, node, mul());
        else if (consume('-')) node = new_binary(ND_SUB, node, mul());
        else return node;
    }
}

static Node *shift(void) {
    Node *node = add();

    for (;;) {
        if (consume(TK_SHL)) node = new_binary(ND_SHL, node, add());
        else if (consume(TK_SHR)) node = new_binary(ND_SHR, node, add());
        else return node;
    }
}

static Node *relational(void) {
    Node *node = shift();

    for (;;) {
        if (consume('<')) node = new_binary(ND_LT, node, shift());
        else if (consume('>')) node = new_binary(ND_GT, node, shift());
        else if (consume(TK_LE)) node = new_binary(ND_LE, node, shift());
        else if (consume(TK_GE)) node = new_binary(ND_GE, node, shift());
        else return node;
    }
}

static Node *equality(void) {
    Node *node = relational();

    for (;;) {
        if (consume(TK_EQ)) node = new_binary(ND_EQ, node, relational());
        else if (consume(TK_NE)) node = new_binary(ND_NE, node, relational());
        else return node;
    }
}

static Node *bitand(void) {
    Node *node = equality();
    while (consume('&')) {
        node = new_binary(ND_BITAND, node, equality());
    }
    return node;
}

static Node *bitxor(void) {
    Node *node = bitand();
    while (consume('^')) {
        node = new_binary(ND_BITXOR, node, bitand());
    }
    return node;
}

static Node *bitor(void) {
    Node *node = bitxor();
    while (consume('|')) {
        node = new_binary(ND_BITOR, node, bitxor());
    }
    return node;
}

static Node *logand(void) {
    Node *node = bitor();
    while (consume(TK_AND)) {
        node = new_binary(ND_LOGAND, node, bitor());
    }
    return node;
}

static Node *logor(void) {
    Node *node = logand();
    while (consume(TK_OR)) {
        node = new_binary(ND_LOGOR, node, logand());
    }
    return node;
}

static Node *assign(void) {
    Node *node = logor();
    if (consume('?')) {
        Node *t = expr();
        expect(':');
        Node *e = assign();
        Node *n = new_node(ND_TERNARY);
        n->cond = node;
        n->then = t;
        n->els = e;
        return n;
    }
    if (consume('=')) {
        node = new_binary(ND_ASSIGN, node, assign());
    } else if (consume(TK_ADD_EQ)) {
        node = new_binary(ND_ADD_EQ, node, assign());
    } else if (consume(TK_SUB_EQ)) {
        node = new_binary(ND_SUB_EQ, node, assign());
    }
    return node;
}

static Node *expr(void) {
    return assign();
}

static long eval_const(Node *node) {
    switch (node->kind) {
        case ND_NUM: return node->val;
        case ND_NEG: return -eval_const(node->lhs);
        case ND_BITNOT: return ~eval_const(node->lhs);
        case ND_ADD: return eval_const(node->lhs) + eval_const(node->rhs);
        case ND_SUB: return eval_const(node->lhs) - eval_const(node->rhs);
        case ND_MUL: return eval_const(node->lhs) * eval_const(node->rhs);
        case ND_DIV: return eval_const(node->lhs) / eval_const(node->rhs);
        case ND_MOD: return eval_const(node->lhs) % eval_const(node->rhs);
        case ND_BITAND: return eval_const(node->lhs) & eval_const(node->rhs);
        case ND_BITOR: return eval_const(node->lhs) | eval_const(node->rhs);
        case ND_BITXOR: return eval_const(node->lhs) ^ eval_const(node->rhs);
        case ND_SHL: return eval_const(node->lhs) << eval_const(node->rhs);
        case ND_SHR: return eval_const(node->lhs) >> eval_const(node->rhs);
        default:
            error_at(token->loc, "case value must be constant");
            return 0;
    }
}

static Node *expr_stmt(void) {
    if (consume(';')) return new_node(ND_NULL);
    Node *node = new_expr_stmt(expr());
    expect(';');
    return node;
}

static Node *declaration_stmt(void) {
    Node head;
    Node *cur = &head;
    head.next = NULL;
    int is_typedef = 0;
    Type *base = NULL;

    if (consume(TK_TYPEDEF)) {
        is_typedef = 1;
    }
    base = basetype();
    {
        int is_static = decl_is_static;
        int is_extern = decl_is_extern;

        for (;;) {
            char *name = NULL;
            Obj *dummy = NULL;
            Type *ty = declarator(base, &name, &dummy);
            if (is_typedef) {
                push_typedef_scope(name, ty);
            } else {
                Obj *var = new_var(name, ty, 1, is_static, is_extern);
                push_scope(name, var);

                if (consume('=')) {
                    Node *assign = new_binary(ND_ASSIGN, new_var_node(var), expr());
                    cur = cur->next = new_expr_stmt(assign);
                }
            }

            if (consume(';')) break;
            expect(',');
        }
    }

    return head.next ? new_block(head.next) : new_node(ND_NULL);
}

static Node *compound_stmt(void) {
    Node head;
    Node *cur = &head;
    head.next = NULL;

    enter_scope();
    while (!consume('}')) {
        if (is_typename()) {
            Node *decl = declaration_stmt();
            if (decl->kind == ND_BLOCK) {
                Node *s;
                for (s = decl->body; s; s = s->next) cur = cur->next = s;
            } else if (decl->kind != ND_NULL) {
                cur = cur->next = decl;
            }
            continue;
        }

        cur = cur->next = stmt();
    }
    leave_scope();
    return new_block(head.next);
}

static Node *stmt(void) {
    if (consume(TK_RETURN)) {
        if (consume(';')) {
            return new_unary(ND_RETURN, NULL);
        } else {
            Node *node = new_unary(ND_RETURN, expr());
            expect(';');
            return node;
        }
    }

    if (consume(TK_IF)) {
        Node *node = new_node(ND_IF);
        expect('(');
        node->cond = expr();
        expect(')');
        node->then = stmt();
        if (consume(TK_ELSE)) node->els = stmt();
        return node;
    }

    if (consume(TK_WHILE)) {
        Node *node = new_node(ND_WHILE);
        expect('(');
        node->cond = expr();
        expect(')');
        loop_depth++;
        node->body = stmt();
        loop_depth--;
        return node;
    }

    if (consume(TK_FOR)) {
        Node *node = new_node(ND_FOR);
        expect('(');
        if (!consume(';')) {
            if (is_typename()) {
                node->init = declaration_stmt();
            } else {
                node->init = expr();
                expect(';');
            }
        }
        if (!consume(';')) {
            node->cond = expr();
            expect(';');
        }
        if (!consume(')')) {
            node->inc = expr();
            expect(')');
        }
        loop_depth++;
        node->body = stmt();
        loop_depth--;
        return node;
    }

    if (consume(TK_BREAK)) {
        if (loop_depth == 0 && !current_switch) error_at(token->loc, "break not in loop/switch");
        expect(';');
        return new_node(ND_BREAK);
    }

    if (consume(TK_CONTINUE)) {
        if (loop_depth == 0) error_at(token->loc, "continue not in loop");
        expect(';');
        return new_node(ND_CONTINUE);
    }

    if (consume(TK_SWITCH)) {
        Node *node = new_node(ND_SWITCH);
        Node *saved = current_switch;
        expect('(');
        node->cond = expr();
        expect(')');
        current_switch = node;
        node->body = stmt();
        current_switch = saved;
        return node;
    }

    if (consume(TK_CASE)) {
        if (!current_switch) error_at(token->loc, "case not in switch");
        Node *val = expr();
        expect(':');
        {
            Node *node = new_node(ND_CASE);
            node->val = eval_const(val);
            node->body = stmt();
            if (!current_switch->case_next) {
                current_switch->case_next = node;
            } else {
                Node *c = current_switch->case_next;
                while (c->case_next) c = c->case_next;
                c->case_next = node;
            }
            return node;
        }
    }

    if (consume(TK_DEFAULT)) {
        if (!current_switch) error_at(token->loc, "default not in switch");
        expect(':');
        {
            Node *node = new_node(ND_CASE);
            node->val = 0;
            node->body = stmt();
            current_switch->default_case = node;
            return node;
        }
    }

    if (consume('{')) {
        return compound_stmt();
    }

    return expr_stmt();
}

Obj *parse(Token *tok) {
    token = tok;
    globals = NULL;
    scope = NULL;
    enter_scope();
    current_switch = NULL;
    loop_depth = 0;

    while (token->kind != TK_EOF) {
        int is_typedef = 0;
        Type *base = NULL;
        if (consume(TK_TYPEDEF)) is_typedef = 1;
        base = basetype();
        {
            int is_static = decl_is_static;
            int is_extern = decl_is_extern;
        if (consume(';')) continue;
        char *name = NULL;
        Obj *params = NULL;
        Type *ty = declarator(base, &name, &params);

        if (ty->kind == TY_FUNC && consume('{')) {
            if (is_typedef) error_at(token->loc, "typedef function definition not allowed");
            Obj *fn = new_var(name, ty, 0, is_static, is_extern);
            fn->is_function = 1;
            locals = NULL;
            current_fn = fn;

            enter_scope();
            fn->params = params;
            {
                Obj *p = params;
                while (p) {
                    push_scope(p->name, p);
                    p = p->next;
                }
            }
            fn->body = compound_stmt();
            fn->locals = locals;
            leave_scope();
            continue;
        }

        if (ty->kind == TY_FUNC) {
            Obj *fn = new_var(name, ty, 0, is_static, is_extern);
            fn->is_function = 1;
            fn->params = params;
            expect(';');
            continue;
        }

        if (is_typedef) {
            push_typedef_scope(name, ty);
            expect(';');
            continue;
        }

        Obj *var = new_var(name, ty, 0, is_static, is_extern);
        if (consume('=')) {
            if (token->kind == TK_STR) {
                Token *st = token;
                token = token->next;
                var->init_data = st->str;
                var->init_len = st->str_len;
            } else if (token->kind == TK_NUM) {
                var->init_data = xcalloc(1, 2);
                var->init_data[0] = (char)(token->val & 0xFF);
                var->init_data[1] = (char)((token->val >> 8) & 0xFF);
                var->init_len = 2;
                token = token->next;
            } else {
                error_at(token->loc, "unsupported initializer");
            }
            var->is_extern = 0;
        }
        expect(';');
        }
    }

    return globals;
}
