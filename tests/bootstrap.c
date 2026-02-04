//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/ast.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




Node *new_node(int kind) {
    Node *node = xcalloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(int kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_unary(int kind, Node *expr) {
    Node *node = new_node(kind);
    node->lhs = expr;
    return node;
}

Node *new_num(long val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    node->ty = ty_int();
    return node;
}

Node *new_var_node(Obj *var) {
    Node *node = new_node(ND_VAR);
    node->var = var;
    node->ty = var->ty;
    return node;
}

Node *new_block(Node *stmts) {
    Node *node = new_node(ND_BLOCK);
    node->body = stmts;
    return node;
}

Node *new_expr_stmt(Node *expr) {
    Node *node = new_node(ND_EXPR_STMT);
    node->lhs = expr;
    return node;
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/codegen.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static FILE *out;
static int labelseq;
static char *current_fn;
static Obj *current_fn_obj;
static int break_labels[32];
static int continue_labels[32];
static int break_depth;
static int continue_depth;

static void emitln(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(out, fmt, ap);
    va_end(ap);
    fprintf(out, "\n");
}

static void emit_br_label(const char *label) {
    emitln("    JMP %s", label);
}

static void emit_beq_label(const char *label) {
    int c = labelseq++;
    emitln("    BNE .Lskip%d", c);
    emitln("    JMP %s", label);
    emitln(".Lskip%d:", c);
}

static void emit_bne_label(const char *label) {
    int c = labelseq++;
    emitln("    BEQ .Lskip%d", c);
    emitln("    JMP %s", label);
    emitln(".Lskip%d:", c);
}

static void emit_br(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_br_label(buf);
}

static void emit_beq(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_beq_label(buf);
}

static void emit_bne(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_bne_label(buf);
}

static void emit_br_name(const char *fmt, const char *name) {
    char buf[64];
    sprintf(buf, fmt, name);
    emit_br_label(buf);
}

static void gen_expr(Node *node);
static void gen_stmt(Node *node);
static void gen_addr(Node *node);

static int inc_amount(Type *ty) {
    if (ty->kind == TY_PTR) return ty->base->size;
    if (ty->kind == TY_ARRAY) return ty->base->size;
    return 1;
}

static void push_r0(void) {
    emitln("    MOV R0, -(R6)");
}

static void pop_r1(void) {
    emitln("    MOV (R6)+, R1");
}

static void push_break(int label) {
    break_labels[break_depth++] = label;
}

static void pop_break(void) {
    if (break_depth > 0) break_depth--;
}

static void push_continue(int label) {
    continue_labels[continue_depth++] = label;
}

static void pop_continue(void) {
    if (continue_depth > 0) continue_depth--;
}

static int current_break(void) {
    return break_depth ? break_labels[break_depth - 1] : -1;
}

static int current_continue(void) {
    return continue_depth ? continue_labels[continue_depth - 1] : -1;
}

static void gen_addr(Node *node) {
    if (node->kind == ND_VAR) {
        if (node->var->is_local) {
            emitln("    MOV R4, R0");
            if (node->var->offset != 0)
                emitln("    ADD #%d, R0", node->var->offset);
            return;
        }
        emitln("    MOV #%s, R0", node->var->asm_name);
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        return;
    }
    if (node->kind == ND_MEMBER) {
        gen_addr(node->lhs);
        if (node->member)
            emitln("    ADD #%d, R0", node->member->offset);
        return;
    }
    fprintf(stderr, "not an lvalue\n");
    exit(1);
}

static void load(Type *ty) {
    if (ty->size == 1) {
        emitln("    MOVB (R0), R0");
    } else {
        emitln("    MOV (R0), R0");
    }
}

static void store(Type *ty) {
    if (ty->size == 1) {
        emitln("    MOVB R0, (R1)");
    } else {
        emitln("    MOV R0, (R1)");
    }
}

static void gen_cmp(Node *node, int kind) {
    int c = labelseq++;

    gen_expr(node->lhs);
    push_r0();
    gen_expr(node->rhs);
    pop_r1();

    emitln("    CMP R0, R1");
    if (kind == ND_EQ)
        emit_beq(".Ltrue%d", c);
    else
        emit_bne(".Ltrue%d", c);

    emitln("    MOV #0, R0");
    emit_br(".Lend%d", c);
    emitln(".Ltrue%d:", c);
    emitln("    MOV #1, R0");
    emitln(".Lend%d:", c);
}

static void gen_rel(Node *node, int kind) {
    int c = labelseq++;

    gen_expr(node->lhs);
    push_r0();
    gen_expr(node->rhs);
    pop_r1();

    emitln("    SUB R0, R1");

    emitln("    MOV R1, R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    BIC #0xFFFE, R0");

    if (kind == ND_LT) {
        return;
    }

    if (kind == ND_GE) {
        emitln("    MOV #1, R1");
        emitln("    SUB R0, R1");
        emitln("    MOV R1, R0");
        return;
    }

    emitln("    CMP #0, R1");
    emit_beq(".Lzero%d", c);
    emitln("    MOV #0, R1");
    emit_br(".Lzero_end%d", c);
    emitln(".Lzero%d:", c);
    emitln("    MOV #1, R1");
    emitln(".Lzero_end%d:", c);

    if (kind == ND_LE) {
        emitln("    BIS R1, R0");
        return;
    }

    if (kind == ND_GT) {
        emitln("    BIS R1, R0");
        emitln("    MOV #1, R1");
        emitln("    SUB R0, R1");
        emitln("    MOV R1, R0");
        return;
    }
}

static void gen_expr(Node *node) {
    int c;

    switch (node->kind) {
        case ND_NUM:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_VAR:
            gen_addr(node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_MEMBER:
            gen_addr(node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_ADDR:
            gen_addr(node->lhs);
            return;
        case ND_DEREF:
            gen_expr(node->lhs);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_PRE_INC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(node->lhs);
            push_r0();
            load(node->lhs->ty);
            if (inc != 0)
                emitln("    ADD #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            return;
        }
        case ND_PRE_DEC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(node->lhs);
            push_r0();
            load(node->lhs->ty);
            if (inc != 0)
                emitln("    SUB #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            return;
        }
        case ND_POST_INC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(node->lhs);
            push_r0();
            load(node->lhs->ty);
            emitln("    MOV R0, R2");
            if (inc != 0)
                emitln("    ADD #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            emitln("    MOV R2, R0");
            return;
        }
        case ND_POST_DEC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(node->lhs);
            push_r0();
            load(node->lhs->ty);
            emitln("    MOV R0, R2");
            if (inc != 0)
                emitln("    SUB #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            emitln("    MOV R2, R0");
            return;
        }
        case ND_ADD_EQ:
        case ND_SUB_EQ: {
            Type *lt = node->lhs->ty;
            int scale = 1;
            int lptr = (lt->kind == TY_PTR || lt->kind == TY_ARRAY);
            if (lptr) scale = lt->base->size;

            gen_addr(node->lhs);
            push_r0();
            load(lt);
            push_r0();
            gen_expr(node->rhs);
            if (lptr && scale == 2) emitln("    ADD R0, R0");
            pop_r1();
            if (node->kind == ND_ADD_EQ)
                emitln("    ADD R0, R1");
            else
                emitln("    SUB R0, R1");
            emitln("    MOV R1, R0");
            pop_r1();
            store(lt);
            return;
        }
        case ND_ASSIGN:
            if (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION) {
                int size = node->lhs->ty->size;
                if (node->rhs->kind == ND_FUNCALL &&
                    (node->rhs->ty->kind == TY_STRUCT || node->rhs->ty->kind == TY_UNION)) {
                    gen_addr(node->lhs);
                    emitln("    MOV R0, R3");
                    gen_expr(node->rhs); /* src addr in R0 */
                    emitln("    MOV R3, R2"); /* dst */
                } else {
                    gen_addr(node->lhs);
                    emitln("    MOV R0, R2"); /* dst */
                    gen_addr(node->rhs);
                }
                emitln("    MOV #%d, R1", size);
                emitln("    MOV R1, -(R6)");
                emitln("    MOV R0, -(R6)");
                emitln("    MOV R2, -(R6)");
                emitln("    JSR R5, __memcpy");
                emitln("    ADD #6, R6");
                if (node->rhs->kind == ND_FUNCALL &&
                    (node->rhs->ty->kind == TY_STRUCT || node->rhs->ty->kind == TY_UNION)) {
                    emitln("    ADD #%d, R6", size); /* pop temp */
                }
                return;
            }
            gen_addr(node->lhs);
            push_r0();
            gen_expr(node->rhs);
            pop_r1();
            store(node->lhs->ty);
            return;
        case ND_NEG:
            gen_expr(node->lhs);
            emitln("    MOV R0, R1");
            emitln("    CLR R0");
            emitln("    SUB R1, R0");
            return;
        case ND_BITNOT:
            gen_expr(node->lhs);
            emitln("    MOV R0, -(R6)");
            emitln("    JSR R5, __not");
            emitln("    ADD #2, R6");
            return;
        case ND_SIZEOF:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_CAST:
            gen_expr(node->lhs);
            if (node->ty && node->ty->size == 1) {
                emitln("    BIC #0xFF00, R0");
            }
            return;
        case ND_ADD:
        case ND_SUB: {
            Type *lt = node->lhs->ty;
            Type *rt = node->rhs->ty;
            int scale = 1;
            int lptr = (lt->kind == TY_PTR || lt->kind == TY_ARRAY);
            int rptr = (rt->kind == TY_PTR || rt->kind == TY_ARRAY);

            if (lptr && rptr && node->kind == ND_SUB) {
                scale = lt->base->size;
                gen_expr(node->lhs);
                push_r0();
                gen_expr(node->rhs);
                pop_r1();
                emitln("    SUB R0, R1");
                if (scale == 2) emitln("    ASR R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (!lptr && rptr && node->kind == ND_ADD) {
                scale = rt->base->size;
                gen_expr(node->rhs);
                push_r0();
                gen_expr(node->lhs);
                if (scale == 2) emitln("    ADD R0, R0");
                pop_r1();
                emitln("    ADD R0, R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (lptr && !rptr) scale = lt->base->size;

            gen_expr(node->lhs);
            push_r0();
            gen_expr(node->rhs);
            if (lptr && scale == 2) emitln("    ADD R0, R0");
            pop_r1();
            if (node->kind == ND_ADD)
                emitln("    ADD R0, R1");
            else
                emitln("    SUB R0, R1");
            emitln("    MOV R1, R0");
            return;
        }
        case ND_MUL:
        case ND_DIV:
        case ND_MOD: {
            gen_expr(node->rhs);
            push_r0();
            gen_expr(node->lhs);
            push_r0();
            if (node->kind == ND_MUL)
                emitln("    JSR R5, __mul");
            else if (node->kind == ND_DIV)
                emitln("    JSR R5, __div");
            else
                emitln("    JSR R5, __mod");
            emitln("    ADD #4, R6");
            return;
        }
        case ND_BITAND:
        case ND_BITOR:
        case ND_BITXOR:
        case ND_SHL:
        case ND_SHR: {
            gen_expr(node->rhs);
            push_r0();
            gen_expr(node->lhs);
            push_r0();
            if (node->kind == ND_BITAND)
                emitln("    JSR R5, __and");
            else if (node->kind == ND_BITOR)
                emitln("    JSR R5, __or");
            else if (node->kind == ND_BITXOR)
                emitln("    JSR R5, __xor");
            else if (node->kind == ND_SHL)
                emitln("    JSR R5, __shl");
            else
                emitln("    JSR R5, __shr");
            emitln("    ADD #4, R6");
            return;
        }
        case ND_EQ:
            gen_cmp(node, ND_EQ);
            return;
        case ND_NE:
            gen_cmp(node, ND_NE);
            return;
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE:
            gen_rel(node, node->kind);
            return;
        case ND_LOGAND:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_beq(".Lfalse%d", c);
            gen_expr(node->rhs);
            emitln("    TST R0");
            emit_beq(".Lfalse%d", c);
            emitln("    MOV #1, R0");
            emit_br(".Lend%d", c);
            emitln(".Lfalse%d:", c);
            emitln("    MOV #0, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_LOGOR:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_bne(".Ltrue%d", c);
            gen_expr(node->rhs);
            emitln("    TST R0");
            emit_bne(".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_NOT:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_beq(".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_TERNARY:
            c = labelseq++;
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lelse%d", c);
            gen_expr(node->then);
            emit_br(".Lend%d", c);
            emitln(".Lelse%d:", c);
            gen_expr(node->els);
            emitln(".Lend%d:", c);
            return;
        case ND_FUNCALL: {
            Node *arg;
            int nargs = 0;
            Node *args[32];
            int is_sret = (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION);
            int size = is_sret ? node->ty->size : 0;

            for (arg = node->args; arg; arg = arg->next) {
                args[nargs++] = arg;
            }

            if (is_sret) {
                emitln("    SUB #%d, R6", size);
                emitln("    MOV R6, R2");
            }

            while (nargs--) {
                gen_expr(args[nargs]);
                push_r0();
            }
            if (is_sret) {
                emitln("    MOV R2, R0");
                push_r0();
            }
            emitln("    JSR R5, %s", node->var->asm_name);
            if (node->args) {
                int count = 0;
                for (arg = node->args; arg; arg = arg->next) count++;
                emitln("    ADD #%d, R6", (count + (is_sret ? 1 : 0)) * 2);
            } else if (is_sret) {
                emitln("    ADD #2, R6");
            }
            if (is_sret) {
                emitln("    MOV R6, R0");
            }
            return;
        }
    }
}

static void gen_stmt(Node *node) {
    int c;

    switch (node->kind) {
        case ND_RETURN:
            if (current_fn_obj && current_fn_obj->has_sret) {
                if (node->lhs) {
                    int size = current_fn_obj->ty->return_ty->size;
                    if (node->lhs->kind == ND_FUNCALL &&
                        (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION)) {
                        emitln("    MOV 4(R4), R3");
                        gen_expr(node->lhs); /* src in R0 */
                        emitln("    MOV R3, R2");
                    } else {
                        emitln("    MOV 4(R4), R2");
                        gen_addr(node->lhs);
                    }
                    emitln("    MOV #%d, R1", size);
                    emitln("    MOV R1, -(R6)");
                    emitln("    MOV R0, -(R6)");
                    emitln("    MOV R2, -(R6)");
                    emitln("    JSR R5, __memcpy");
                    emitln("    ADD #6, R6");
                    if (node->lhs->kind == ND_FUNCALL &&
                        (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION)) {
                        emitln("    ADD #%d, R6", size);
                    }
                }
                emit_br_name(".Lreturn_%s", current_fn);
                return;
            }
            if (node->lhs) gen_expr(node->lhs);
            emit_br_name(".Lreturn_%s", current_fn);
            return;
        case ND_IF:
            c = labelseq++;
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lelse%d", c);
            gen_stmt(node->then);
            emit_br(".Lend%d", c);
            emitln(".Lelse%d:", c);
            if (node->els) gen_stmt(node->els);
            emitln(".Lend%d:", c);
            return;
        case ND_WHILE:
            c = labelseq++;
            emitln(".Lbegin%d:", c);
            emitln(".Lcontinue%d:", c);
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lend%d", c);
            push_break(c);
            push_continue(c);
            gen_stmt(node->body);
            pop_continue();
            pop_break();
            emit_br(".Lbegin%d", c);
            emitln(".Lend%d:", c);
            return;
        case ND_FOR: {
            int begin = labelseq++;
            int end = labelseq++;
            int cont = labelseq++;
            if (node->init) {
                if (node->init->kind == ND_BLOCK || node->init->kind == ND_EXPR_STMT || node->init->kind == ND_NULL)
                    gen_stmt(node->init);
                else
                    gen_expr(node->init);
            }
            emitln(".Lbegin%d:", begin);
            if (node->cond) {
                gen_expr(node->cond);
                emitln("    TST R0");
                emit_beq(".Lend%d", end);
            }
            push_break(end);
            push_continue(cont);
            gen_stmt(node->body);
            emitln(".Lcontinue%d:", cont);
            if (node->inc) gen_expr(node->inc);
            pop_continue();
            pop_break();
            emit_br(".Lbegin%d", begin);
            emitln(".Lend%d:", end);
            return;
        }
        case ND_BREAK:
            if (current_break() >= 0)
                emit_br(".Lend%d", current_break());
            return;
        case ND_CONTINUE:
            if (current_continue() >= 0)
                emit_br(".Lcontinue%d", current_continue());
            return;
        case ND_SWITCH: {
            int end = labelseq++;
            Node *cnode;
            gen_expr(node->cond);
            for (cnode = node->case_next; cnode; cnode = cnode->case_next) {
                cnode->case_label = labelseq++;
                emitln("    CMP #%ld, R0", cnode->val);
                emit_beq(".Lcase%d", cnode->case_label);
            }
            if (node->default_case) {
                node->default_case->case_label = labelseq++;
                emit_br(".Lcase%d", node->default_case->case_label);
            } else {
                emit_br(".Lend%d", end);
            }
            push_break(end);
            gen_stmt(node->body);
            pop_break();
            emitln(".Lend%d:", end);
            return;
        }
        case ND_CASE:
            emitln(".Lcase%d:", node->case_label);
            if (node->body) gen_stmt(node->body);
            return;
        case ND_BLOCK: {
            Node *stmt;
            for (stmt = node->body; stmt; stmt = stmt->next) {
                gen_stmt(stmt);
            }
            return;
        }
        case ND_EXPR_STMT:
            if (node->lhs) gen_expr(node->lhs);
            return;
        case ND_NULL:
            return;
    }
}

static void emit_global_data(Obj *var) {
    int total = var->ty->size;
    int i = 0;
    if (var->is_extern && !var->init_data) return;
    emitln("%s:", var->asm_name);
    while (i < total) {
        int lo = 0;
        int hi = 0;
        if (var->init_data && i < var->init_len) lo = (unsigned char)var->init_data[i];
        if (var->init_data && i + 1 < var->init_len) hi = (unsigned char)var->init_data[i + 1];
        emitln("    .WORD 0x%04X", (hi << 8) | lo);
        i += 2;
    }
}

static void emit_runtime(void) {
    emitln("putchar:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #1");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("getchar:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    TRAP #2");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("puts:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #3");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__memcpy:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* dst */
    emitln("    MOV 6(R4), R1"); /* src */
    emitln("    MOV 8(R4), R2"); /* size */
    emitln(".Lmemcpy_loop:");
    emitln("    TST R2");
    emitln("    BEQ .Lmemcpy_end");
    emitln("    MOVB (R1)+, (R0)+");
    emitln("    DEC R2");
    emitln("    BR .Lmemcpy_loop");
    emitln(".Lmemcpy_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__not:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R1");
    emitln("    MOV #0xFFFF, R0");
    emitln("    BIC R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__and:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV #0xFFFF, R2");
    emitln("    BIC R0, R2");
    emitln("    MOV #0xFFFF, R3");
    emitln("    BIC R1, R3");
    emitln("    MOV R2, R0");
    emitln("    BIS R3, R0");
    emitln("    MOV #0xFFFF, R1");
    emitln("    BIC R0, R1");
    emitln("    MOV R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__or:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    BIS R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__xor:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R2");
    emitln("    BIC R1, R2");
    emitln("    MOV R1, R3");
    emitln("    BIC R0, R3");
    emitln("    BIS R2, R3");
    emitln("    MOV R3, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__shl:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln(".Lshl_loop:");
    emitln("    TST R1");
    emitln("    BEQ .Lshl_end");
    emitln("    ASL R0");
    emitln("    DEC R1");
    emitln("    BR .Lshl_loop");
    emitln(".Lshl_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__shr:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln(".Lshr_loop:");
    emitln("    TST R1");
    emitln("    BEQ .Lshr_end");
    emitln("    ASR R0");
    emitln("    DEC R1");
    emitln("    BR .Lshr_loop");
    emitln(".Lshr_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__mul:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    CLR R2");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmul_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, R2");
    emitln(".Lmul_a_pos:");
    emitln("    MOV R1, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmul_b_pos");
    emitln("    MOV R1, R3");
    emitln("    CLR R1");
    emitln("    SUB R3, R1");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, R2");
    emitln(".Lmul_b_pos:");
    emitln("    CLR R3");
    emitln(".Lmul_loop:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Lmul_done");
    emitln("    ADD R0, R3");
    emitln("    DEC R1");
    emitln("    BR .Lmul_loop");
    emitln(".Lmul_done:");
    emitln("    MOV R3, R0");
    emitln("    CMP #0, R2");
    emitln("    BEQ .Lmul_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Lmul_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__div:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    SUB #2, R6");
    emitln("    CLR -2(R4)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV -2(R4), R2");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Ldiv_a_pos:");
    emitln("    MOV R1, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_b_pos");
    emitln("    MOV R1, R3");
    emitln("    CLR R1");
    emitln("    SUB R3, R1");
    emitln("    MOV -2(R4), R2");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Ldiv_b_pos:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Ldiv_zero");
    emitln("    CLR R2");
    emitln(".Ldiv_loop:");
    emitln("    MOV R0, R3");
    emitln("    SUB R1, R3");
    emitln("    MOV R3, R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    BIC #0xFFFE, R0");
    emitln("    CMP #0, R0");
    emitln("    BNE .Ldiv_done");
    emitln("    MOV R3, R0");
    emitln("    INC R2");
    emitln("    BR .Ldiv_loop");
    emitln(".Ldiv_done:");
    emitln("    MOV R2, R0");
    emitln("    MOV -2(R4), R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Ldiv_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Ldiv_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__mod:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    SUB #2, R6");
    emitln("    CLR -2(R4)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmod_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV #1, R3");
    emitln("    SUB -2(R4), R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Lmod_a_pos:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Lmod_zero");
    emitln(".Lmod_loop:");
    emitln("    MOV R0, R3");
    emitln("    SUB R1, R3");
    emitln("    MOV R3, R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    BIC #0xFFFE, R0");
    emitln("    CMP #0, R0");
    emitln("    BNE .Lmod_done");
    emitln("    MOV R3, R0");
    emitln("    BR .Lmod_loop");
    emitln(".Lmod_done:");
    emitln("    MOV -2(R4), R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmod_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Lmod_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Lmod_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
}

void codegen(Obj *prog, FILE *outfp) {
    Obj *fn;

    out = outfp;
    emitln(".ORIG 0x1000");
    emitln("_start:");
    emitln("    MOV #0xFFFE, R6");
    emitln("    JSR R5, main");
    emitln("    HALT");

    for (fn = prog; fn; fn = fn->next) {
        if (!fn->is_function) continue;
        if (!fn->body) continue;
        current_fn = fn->asm_name;
        current_fn_obj = fn;
        emitln("%s:", fn->asm_name);
        emitln("    MOV R4, -(R6)");
        emitln("    MOV R6, R4");
        if (fn->stack_size)
            emitln("    SUB #%d, R6", fn->stack_size);
        gen_stmt(fn->body);
        emitln(".Lreturn_%s:", fn->asm_name);
        emitln("    MOV R4, R6");
        emitln("    MOV (R6)+, R4");
        emitln("    RTS R5");
    }

    for (fn = prog; fn; fn = fn->next) {
        if (fn->is_function) continue;
        emit_global_data(fn);
    }

    emit_runtime();
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/main_sim.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static char *read_all_stdin(void) {
    size_t cap = 4096;
    size_t len = 0;
    char *buf = xcalloc(1, cap);
    int c;

    while ((c = getchar()) != -1) {
        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = (char)c;
    }
    if (len == 0 || buf[len - 1] != '\n') {
        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = '\n';
    }
    buf[len] = '\0';
    return buf;
}

static Obj *append_prog(Obj *all, Obj *prog) {
    Obj *tail;
    if (!all) return prog;
    tail = all;
    while (tail->next) tail = tail->next;
    tail->next = prog;
    return all;
}

static void parse_segment(char *start, char *end, int file_id, Obj **all) {
    char *buf;
    char *p;
    char *prefix;
    Token *tok;
    Obj *prog;

    if (!start || start >= end) return;

    buf = xcalloc(1, (size_t)(end - start + 2));
    p = buf;
    while (start < end) *p++ = *start++;
    *p = '\0';

    prefix = xcalloc(1, 32);
    sprintf(prefix, "F%d_", file_id);
    set_file_prefix(prefix);

    tok = tokenize(buf);
    prog = parse(tok);
    *all = append_prog(*all, prog);
}

int main(int argc, char **argv) {
    char *input;
    char *p;
    char *line;
    char *seg_start = NULL;
    Obj *all = NULL;
    int file_id = -1;

    (void)argc;
    (void)argv;

    input = read_all_stdin();

    line = input;
    p = input;
    while (*p) {
        if (*p == '\n') {
            *p = '\0';
            if (startswith(line, "//--FILE:")) {
                if (file_id >= 0 && seg_start) {
                    parse_segment(seg_start, line - 1, file_id, &all);
                }
                file_id++;
                seg_start = p + 1;
            }
            *p = '\n';
            p++;
            line = p;
            continue;
        }
        p++;
    }

    if (file_id >= 0 && seg_start) {
        parse_segment(seg_start, p, file_id, &all);
    } else {
        /* No markers: treat whole input as a single file. */
        file_id = 0;
        parse_segment(input, p, file_id, &all);
    }

    sema(all);
    codegen(all, stdout);
    return 0;
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/parser.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);





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

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/sema.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

static void add_type(Node *node) {
    if (!node || node->ty) return;

    if (node->kind == ND_BLOCK) {
        Node *n;
        for (n = node->body; n; n = n->next) add_type(n);
        node->ty = ty_void();
        return;
    }

    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->els);
    add_type(node->init);
    add_type(node->inc);
    add_type(node->body);
    {
        Node *arg;
        for (arg = node->args; arg; arg = arg->next) add_type(arg);
    }

    switch (node->kind) {
        case ND_NUM:
            node->ty = ty_int();
            return;
        case ND_VAR:
            node->ty = node->var->ty;
            return;
        case ND_ADD:
        case ND_SUB:
            if (node->lhs->ty->kind == TY_PTR || node->lhs->ty->kind == TY_ARRAY) {
                node->ty = node->lhs->ty->kind == TY_ARRAY ? pointer_to(node->lhs->ty->base) : node->lhs->ty;
                return;
            }
            node->ty = ty_int();
            return;
        case ND_MUL:
        case ND_DIV:
        case ND_MOD:
        case ND_BITAND:
        case ND_BITOR:
        case ND_BITXOR:
        case ND_SHL:
        case ND_SHR:
            node->ty = ty_int();
            return;
        case ND_NEG:
        case ND_PRE_INC:
        case ND_PRE_DEC:
        case ND_POST_INC:
        case ND_POST_DEC:
            node->ty = node->lhs->ty;
            return;
        case ND_CAST:
            node->ty = node->cast_ty ? node->cast_ty : node->lhs->ty;
            return;
        case ND_ASSIGN:
        case ND_ADD_EQ:
        case ND_SUB_EQ:
            node->ty = node->lhs->ty;
            return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE:
        case ND_LOGAND:
        case ND_LOGOR:
        case ND_NOT:
        case ND_BITNOT:
        case ND_SIZEOF:
        case ND_TERNARY:
            node->ty = ty_int();
            if (node->kind == ND_SIZEOF && node->lhs)
                node->val = node->lhs->ty->size;
            return;
        case ND_ADDR:
            if (node->lhs->ty->kind == TY_ARRAY) {
                node->ty = pointer_to(node->lhs->ty->base);
            } else {
                node->ty = pointer_to(node->lhs->ty);
            }
            return;
        case ND_DEREF:
            if (node->lhs->ty->kind == TY_PTR)
                node->ty = node->lhs->ty->base;
            else if (node->lhs->ty->kind == TY_ARRAY)
                node->ty = node->lhs->ty->base;
            else
                node->ty = ty_int();
            return;
        case ND_MEMBER: {
            Type *ty = node->lhs->ty;
            if (ty->kind == TY_PTR) ty = ty->base;
            if (ty->kind != TY_STRUCT && ty->kind != TY_UNION) {
                fprintf(stderr, "not a struct/union\n");
                exit(1);
            }
            node->member = find_member(ty, node->member_name);
            if (!node->member) { fprintf(stderr, "no such member: %s\n", node->member_name); exit(1); }
            node->ty = node->member->ty;
            return;
        }
        case ND_FUNCALL:
            if (node->var && node->var->ty && node->var->ty->kind == TY_FUNC)
                node->ty = node->var->ty->return_ty;
            else
                node->ty = ty_int();
            return;
        case ND_RETURN:
            node->ty = node->lhs ? node->lhs->ty : ty_void();
            return;
        case ND_EXPR_STMT:
            node->ty = node->lhs ? node->lhs->ty : ty_void();
            return;
        case ND_BLOCK:
        case ND_IF:
        case ND_WHILE:
        case ND_FOR:
        case ND_SWITCH:
        case ND_CASE:
        case ND_BREAK:
        case ND_CONTINUE:
        case ND_NULL:
            node->ty = ty_void();
            return;
    }
}

static void assign_lvar_offsets(Obj *fn) {
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

void sema(Obj *prog) {
    Obj *fn;
    for (fn = prog; fn; fn = fn->next) {
        if (!fn->is_function) continue;
        assign_lvar_offsets(fn);
        add_type(fn->body);
    }
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/sim_support.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);



/* Declarations provided by runtime assembly emitted by codegen. */
int putchar(int c);
int getchar(void);

/* Minimal FILE stub */
typedef struct FILE FILE;
struct FILE { int dummy; };
FILE *stdin = 0;
FILE *stdout = 0;
FILE *stderr = 0;

/* Simple bump allocator with size headers. */
static unsigned char heap[HEAP_SIZE];
static unsigned int heap_pos;

static void *alloc_raw(size_t size) {
    unsigned int total;
    unsigned int *hdr;

    if (size == 0) size = 1;
    if (size & 1) size++;
    total = (unsigned int)size + (unsigned int)sizeof(unsigned int);
    if (heap_pos + total > HEAP_SIZE) {
        /* out of memory: spin */
        for (;;) {}
    }
    hdr = (unsigned int *)(heap + heap_pos);
    *hdr = (unsigned int)size;
    heap_pos += total;
    return (void *)(hdr + 1);
}

void *malloc(size_t size) {
    return alloc_raw(size);
}

void *calloc(size_t nmemb, size_t size) {
    size_t n = nmemb * size;
    unsigned char *p = (unsigned char *)alloc_raw(n);
    size_t i;
    for (i = 0; i < n; i++) p[i] = 0;
    return p;
}

void free(void *ptr) {
    (void)ptr;
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) return alloc_raw(size);
    {
        unsigned int *hdr = ((unsigned int *)ptr) - 1;
        unsigned int old = *hdr;
        if (size <= old) return ptr;
        {
            unsigned char *p = (unsigned char *)alloc_raw(size);
            unsigned int i;
            for (i = 0; i < old; i++) p[i] = ((unsigned char *)ptr)[i];
            return p;
        }
    }
}

void exit(int code) {
    (void)code;
    for (;;) {}
}

/* Ctype */
int isspace(int c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

/* String/memory */
size_t strlen(const char *s) {
    size_t n = 0;
    while (s && s[n]) n++;
    return n;
}

int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
    size_t i = 0;
    if (n == 0) return 0;
    while (i + 1 < n && a[i] && b[i] && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    return c == 0 ? (char *)s : NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == 0) return (char *)s;
    return (char *)last;
}

char *strstr(const char *hay, const char *needle) {
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    while (*hay) {
        if (strncmp(hay, needle, nlen) == 0) return (char *)hay;
        hay++;
    }
    return NULL;
}

void *memcpy(void *dst, const void *src, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = ((const unsigned char *)src)[i];
    return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
    size_t i;
    if (dst < src) {
        for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = ((const unsigned char *)src)[i];
    } else if (dst > src) {
        for (i = n; i > 0; i--) ((unsigned char *)dst)[i - 1] = ((const unsigned char *)src)[i - 1];
    }
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) ((unsigned char *)dst)[i] = (unsigned char)c;
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    size_t i;
    for (i = 0; i < n; i++) {
        int da = ((const unsigned char *)a)[i];
        int db = ((const unsigned char *)b)[i];
        if (da != db) return da - db;
    }
    return 0;
}

/* Minimal stdio stubs */
FILE *fopen(const char *path, const char *mode) {
    (void)path;
    (void)mode;
    return NULL;
}

int fclose(FILE *fp) {
    (void)fp;
    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp) {
    (void)fp;
    size_t total = size * nmemb;
    size_t i = 0;
    int c;
    unsigned char *p = (unsigned char *)ptr;
    while (i < total) {
        c = getchar();
        if (c == -1) break;
        p[i++] = (unsigned char)c;
    }
    return (size == 0) ? 0 : (i / size);
}

int fseek(FILE *fp, long off, int whence) {
    (void)fp; (void)off; (void)whence;
    return -1;
}

long ftell(FILE *fp) {
    (void)fp;
    return -1;
}

/* Formatting */
struct OutBuf {
    char *buf;
    int idx;
    int to_buf;
};

static void outch(struct OutBuf *o, int c) {
    if (o->to_buf) {
        o->buf[o->idx++] = (char)c;
    } else {
        putchar(c);
    }
}

static void outstr(struct OutBuf *o, const char *s, int maxlen) {
    int i = 0;
    if (!s) s = "(null)";
    while (s[i] && (maxlen < 0 || i < maxlen)) {
        outch(o, s[i]);
        i++;
    }
}

static void outpad(struct OutBuf *o, int count, int ch) {
    while (count-- > 0) outch(o, ch);
}

static void outnum(struct OutBuf *o, unsigned long v, int base, int upper, int width, int pad_zero, int neg) {
    char tmp[32];
    int idx = 0;
    int pad = 0;

    if (v == 0) tmp[idx++] = '0';
    while (v) {
        int d = (int)(v % (unsigned long)base);
        if (d < 10) tmp[idx++] = (char)('0' + d);
        else tmp[idx++] = (char)((upper ? 'A' : 'a') + (d - 10));
        v = v / (unsigned long)base;
    }
    if (neg) tmp[idx++] = '-';

    pad = width - idx;
    if (pad > 0) outpad(o, pad, pad_zero ? '0' : ' ');
    while (idx--) outch(o, tmp[idx]);
}

static int mini_vformat(struct OutBuf *o, const char *fmt, va_list ap) {
    const char *p = fmt;
    while (*p) {
        if (*p != '%') {
            outch(o, *p++);
            continue;
        }
        p++;
        if (*p == '%') { outch(o, '%'); p++; continue; }

        int pad_zero = 0;
        int width = 0;
        int width_from_arg = 0;
        int precision = -1;
        int prec_from_arg = 0;
        int longflag = 0;

        if (*p == '0') { pad_zero = 1; p++; }

        if (*p == '*') {
            width_from_arg = 1;
            p++;
        } else {
            while (*p >= '0' && *p <= '9') {
                width = width * 10 + (*p - '0');
                p++;
            }
        }

        if (*p == '.') {
            p++;
            precision = 0;
            if (*p == '*') {
                prec_from_arg = 1;
                p++;
            } else {
                while (*p >= '0' && *p <= '9') {
                    precision = precision * 10 + (*p - '0');
                    p++;
                }
            }
        }

        if (*p == 'l') { longflag = 1; p++; }

        if (width_from_arg) width = va_arg(ap, int);
        if (prec_from_arg) precision = va_arg(ap, int);

        switch (*p) {
            case 'd': {
                long v = longflag ? va_arg(ap, long) : va_arg(ap, int);
                unsigned long uv = (v < 0) ? (unsigned long)(-v) : (unsigned long)v;
                outnum(o, uv, 10, 0, width, pad_zero, v < 0);
                break;
            }
            case 'u': {
                unsigned long v = longflag ? va_arg(ap, unsigned long) : (unsigned long)va_arg(ap, unsigned int);
                outnum(o, v, 10, 0, width, pad_zero, 0);
                break;
            }
            case 'x':
            case 'X': {
                unsigned long v = longflag ? va_arg(ap, unsigned long) : (unsigned long)va_arg(ap, unsigned int);
                outnum(o, v, 16, *p == 'X', width, pad_zero, 0);
                break;
            }
            case 'c': {
                int ch = va_arg(ap, int);
                outch(o, ch);
                break;
            }
            case 's': {
                const char *s = va_arg(ap, const char *);
                int len = (int)strlen(s ? s : "(null)");
                int n = (precision >= 0 && precision < len) ? precision : len;
                if (width > n) outpad(o, width - n, ' ');
                outstr(o, s, n);
                break;
            }
            default:
                outch(o, '%');
                outch(o, *p);
                break;
        }
        p++;
    }
    return 0;
}

int vfprintf(FILE *fp, const char *fmt, va_list ap) {
    struct OutBuf o;
    (void)fp;
    o.buf = NULL;
    o.idx = 0;
    o.to_buf = 0;
    mini_vformat(&o, fmt, ap);
    return 0;
}

int fprintf(FILE *fp, const char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    return 0;
}

int printf(const char *fmt) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    return 0;
}

int sprintf(char *buf, const char *fmt) {
    va_list ap;
    struct OutBuf o;
    va_start(ap, fmt);
    o.buf = buf;
    o.idx = 0;
    o.to_buf = 1;
    mini_vformat(&o, fmt, ap);
    o.buf[o.idx] = '\0';
    va_end(ap);
    return o.idx;
}

int __va_arg(va_list *ap) {
    int v = *(int *)(*ap);
    *ap += 2;
    return v;
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/tokenize.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static Token *new_token(int kind, char *start, char *end) {
    Token *tok = xcalloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = (int)(end - start);
    return tok;
}

static int is_ident1(int c) {
    return isalpha(c) || c == '_';
}

static int is_ident2(int c) {
    return isalnum(c) || c == '_';
}

static long read_number(char **p) {
    char *s = *p;
    int base = 10;
    long val = 0;

    if (s[0] == '0') {
        if (s[1] == 'x' || s[1] == 'X') {
            base = 16;
            s += 2;
        } else if (isdigit((unsigned char)s[1])) {
            base = 8;
            s += 1;
        }
    }

    while (*s) {
        int d;
        if (isdigit((unsigned char)*s)) d = *s - '0';
        else if (*s >= 'a' && *s <= 'f') d = 10 + (*s - 'a');
        else if (*s >= 'A' && *s <= 'F') d = 10 + (*s - 'A');
        else break;
        if (d >= base) break;
        val = val * base + d;
        s++;
    }

    *p = s;
    return val;
}

static int read_escape(char **p) {
    char *s = *p;
    if (*s == 'n') { *p = s + 1; return '\n'; }
    if (*s == 't') { *p = s + 1; return '\t'; }
    if (*s == 'r') { *p = s + 1; return '\r'; }
    if (*s == '\\') { *p = s + 1; return '\\'; }
    if (*s == '\'') { *p = s + 1; return '\''; }
    if (*s == '\"') { *p = s + 1; return '\"'; }
    *p = s + 1;
    return *s;
}

Token *tokenize(char *input) {
    Token head;
    Token *cur = &head;
    char *p = input;

    set_current_input(input);

    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }

        if (startswith(p, "//")) {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) error_at(p, "unterminated comment");
            p = q + 2;
            continue;
        }

        if (*p == '"') {
            char *start = p++;
            char *buf = xcalloc(1, 4096);
            int len = 0;
            while (*p && *p != '"') {
                if (*p == '\\') {
                    p++;
                    buf[len++] = (char)read_escape(&p);
                    continue;
                }
                buf[len++] = *p++;
            }
            if (*p != '"') error_at(start, "unterminated string");
            p++;
            Token *tok = new_token(TK_STR, start, p);
            tok->str = buf;
            tok->str_len = len;
            cur = cur->next = tok;
            continue;
        }

        if (*p == '\'') {
            char *start = p++;
            int c;
            if (*p == '\\') {
                p++;
                c = read_escape(&p);
            } else {
                c = *p++;
            }
            if (*p != '\'') error_at(start, "unterminated char literal");
            p++;
            Token *tok = new_token(TK_CHAR, start, p);
            tok->val = c;
            cur = cur->next = tok;
            continue;
        }

        if (isdigit((unsigned char)*p)) {
            char *start = p;
            long val = read_number(&p);
            Token *tok = new_token(TK_NUM, start, p);
            tok->val = val;
            cur = cur->next = tok;
            continue;
        }

        if (is_ident1(*p)) {
            char *start = p;
            p++;
            while (is_ident2(*p)) p++;
            Token *tok = new_token(TK_IDENT, start, p);
            tok->str = strndup2(start, tok->len);

            if (tok->len == 6 && strncmp(tok->str, "return", 6) == 0) tok->kind = TK_RETURN;
            else if (tok->len == 2 && strncmp(tok->str, "if", 2) == 0) tok->kind = TK_IF;
            else if (tok->len == 4 && strncmp(tok->str, "else", 4) == 0) tok->kind = TK_ELSE;
            else if (tok->len == 5 && strncmp(tok->str, "while", 5) == 0) tok->kind = TK_WHILE;
            else if (tok->len == 3 && strncmp(tok->str, "for", 3) == 0) tok->kind = TK_FOR;
            else if (tok->len == 5 && strncmp(tok->str, "break", 5) == 0) tok->kind = TK_BREAK;
            else if (tok->len == 8 && strncmp(tok->str, "continue", 8) == 0) tok->kind = TK_CONTINUE;
            else if (tok->len == 6 && strncmp(tok->str, "switch", 6) == 0) tok->kind = TK_SWITCH;
            else if (tok->len == 4 && strncmp(tok->str, "case", 4) == 0) tok->kind = TK_CASE;
            else if (tok->len == 7 && strncmp(tok->str, "default", 7) == 0) tok->kind = TK_DEFAULT;
            else if (tok->len == 3 && strncmp(tok->str, "int", 3) == 0) tok->kind = TK_INT;
            else if (tok->len == 4 && strncmp(tok->str, "char", 4) == 0) tok->kind = TK_CHAR_KW;
            else if (tok->len == 4 && strncmp(tok->str, "void", 4) == 0) tok->kind = TK_VOID;
            else if (tok->len == 6 && strncmp(tok->str, "struct", 6) == 0) tok->kind = TK_STRUCT;
            else if (tok->len == 5 && strncmp(tok->str, "union", 5) == 0) tok->kind = TK_UNION;
            else if (tok->len == 4 && strncmp(tok->str, "enum", 4) == 0) tok->kind = TK_ENUM;
            else if (tok->len == 6 && strncmp(tok->str, "sizeof", 6) == 0) tok->kind = TK_SIZEOF;
            else if (tok->len == 7 && strncmp(tok->str, "typedef", 7) == 0) tok->kind = TK_TYPEDEF;
            else if (tok->len == 6 && strncmp(tok->str, "static", 6) == 0) tok->kind = TK_STATIC;
            else if (tok->len == 6 && strncmp(tok->str, "extern", 6) == 0) tok->kind = TK_EXTERN;
            else if (tok->len == 5 && strncmp(tok->str, "const", 5) == 0) tok->kind = TK_CONST;
            else if (tok->len == 8 && strncmp(tok->str, "volatile", 8) == 0) tok->kind = TK_VOLATILE;
            else if (tok->len == 6 && strncmp(tok->str, "signed", 6) == 0) tok->kind = TK_SIGNED;
            else if (tok->len == 8 && strncmp(tok->str, "unsigned", 8) == 0) tok->kind = TK_UNSIGNED;
            else if (tok->len == 5 && strncmp(tok->str, "short", 5) == 0) tok->kind = TK_SHORT;
            else if (tok->len == 4 && strncmp(tok->str, "long", 4) == 0) tok->kind = TK_LONG;

            cur = cur->next = tok;
            continue;
        }

        if (startswith(p, "...")) { cur = cur->next = new_token(TK_ELLIPSIS, p, p + 3); p += 3; continue; }
        if (startswith(p, "++")) { cur = cur->next = new_token(TK_INC, p, p + 2); p += 2; continue; }
        if (startswith(p, "+=")) { cur = cur->next = new_token(TK_ADD_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "--")) { cur = cur->next = new_token(TK_DEC, p, p + 2); p += 2; continue; }
        if (startswith(p, "-=")) { cur = cur->next = new_token(TK_SUB_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "->")) { cur = cur->next = new_token(TK_ARROW, p, p + 2); p += 2; continue; }
        if (startswith(p, "==")) { cur = cur->next = new_token(TK_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "!=")) { cur = cur->next = new_token(TK_NE, p, p + 2); p += 2; continue; }
        if (startswith(p, "<=")) { cur = cur->next = new_token(TK_LE, p, p + 2); p += 2; continue; }
        if (startswith(p, ">=")) { cur = cur->next = new_token(TK_GE, p, p + 2); p += 2; continue; }
        if (startswith(p, "&&")) { cur = cur->next = new_token(TK_AND, p, p + 2); p += 2; continue; }
        if (startswith(p, "||")) { cur = cur->next = new_token(TK_OR, p, p + 2); p += 2; continue; }
        if (startswith(p, "<<")) { cur = cur->next = new_token(TK_SHL, p, p + 2); p += 2; continue; }
        if (startswith(p, ">>")) { cur = cur->next = new_token(TK_SHR, p, p + 2); p += 2; continue; }

        if (strchr("+-*/%(){}[];,<>=!&|.^~:?", *p)) {
            cur = cur->next = new_token(*p, p, p + 1);
            p++;
            continue;
        }

        error_at(p, "invalid token");
    }

    cur->next = new_token(TK_EOF, p, p);
    return head.next;
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/type.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static Type ty_int_obj;
static Type ty_char_obj;
static Type ty_void_obj;

Type *ty_int(void) {
    ty_int_obj.kind = TY_INT;
    ty_int_obj.size = 2;
    ty_int_obj.align = 2;
    return &ty_int_obj;
}

Type *ty_char(void) {
    ty_char_obj.kind = TY_CHAR;
    ty_char_obj.size = 1;
    ty_char_obj.align = 1;
    return &ty_char_obj;
}

Type *ty_void(void) {
    ty_void_obj.kind = TY_VOID;
    ty_void_obj.size = 1;
    ty_void_obj.align = 1;
    return &ty_void_obj;
}

Type *pointer_to(Type *base) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_PTR;
    ty->base = base;
    ty->size = 2;
    ty->align = 2;
    return ty;
}

Type *array_of(Type *base, int len) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->base = base;
    ty->array_len = len;
    ty->size = base->size * len;
    ty->align = base->align;
    return ty;
}

Type *func_type(Type *ret, Type *params) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_FUNC;
    ty->return_ty = ret;
    ty->params = params;
    ty->is_func = 1;
    ty->size = 2;
    ty->align = 2;
    return ty;
}

int is_integer(Type *ty) {
    return ty->kind == TY_INT || ty->kind == TY_CHAR;
}

static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

Type *struct_type(int is_union, char *tag, Member *members) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = is_union ? TY_UNION : TY_STRUCT;
    ty->tag = tag;
    ty->members = members;

    if (!members) {
        ty->is_incomplete = 1;
        ty->size = 0;
        ty->align = 1;
        return ty;
    }

    if (is_union) {
        int max = 0;
        int align = 1;
        Member *m;
        for (m = members; m; m = m->next) {
            if (m->ty->size > max) max = m->ty->size;
            if (m->ty->align > align) align = m->ty->align;
            m->offset = 0;
        }
        ty->align = align;
        ty->size = align_to(max, align);
        return ty;
    }

    {
        int offset = 0;
        int align = 1;
        Member *m;
        for (m = members; m; m = m->next) {
            offset = align_to(offset, m->ty->align);
            m->offset = offset;
            offset += m->ty->size;
            if (m->ty->align > align) align = m->ty->align;
        }
        ty->align = align;
        ty->size = align_to(offset, align);
    }
    return ty;
}

Member *find_member(Type *ty, const char *name) {
    Member *m;
    for (m = ty->members; m; m = m->next) {
        if (strcmp(m->name, name) == 0) return m;
    }
    return NULL;
}

//--FILE:/Users/donaldmolaro/src/pdp11-c89/src/util.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static char *current_input;

void error_at(char *loc, const char *fmt, ...) {
    va_list ap;
    int pos;
    char *line_start = loc;
    char *line_end = loc;

    while (current_input < line_start && line_start[-1] != '\n') line_start--;
    while (*line_end && *line_end != '\n') line_end++;
    pos = loc - line_start;
    fprintf(stderr, "%.*s\n", (int)(line_end - line_start), line_start);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

void *xcalloc(size_t nmemb, size_t size) {
    void *p = calloc(nmemb, size);
    if (!p) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    return p;
}

char *strndup2(const char *p, int len) {
    char *s = xcalloc(1, len + 1);
    memcpy(s, p, len);
    s[len] = '\0';
    return s;
}

int startswith(const char *p, const char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

void set_current_input(char *input) {
    current_input = input;
}

//--FILE:src/main_sim.c

typedef char* va_list;
int __va_arg(va_list *ap);



typedef unsigned short size_t;




typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);




void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void exit(int status);




void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);



int isspace(int c);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);




/* Tokenizer */

typedef struct Token Token;
struct Token {
    int kind;
    Token *next;
    char *loc;
    int len;
    long val;
    char *str;
    int str_len;
};

enum {
    TK_EOF = 256,
    TK_IDENT,
    TK_NUM,
    TK_STR,
    TK_CHAR,

    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_AND,
    TK_OR,
    TK_SHL,
    TK_SHR,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_INC,
    TK_DEC,
    TK_ARROW,
    TK_SIZEOF,
    TK_ELLIPSIS,
    TK_TYPEDEF,
    TK_STATIC,
    TK_EXTERN,
    TK_CONST,
    TK_VOLATILE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_SHORT,
    TK_LONG,

    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_BREAK,
    TK_CONTINUE,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_INT,
    TK_CHAR_KW,
    TK_VOID,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM
};

Token *tokenize(char *input);
void error_at(char *loc, const char *fmt, ...);
void set_current_input(char *input);
void set_file_prefix(const char *prefix);

char *preprocess_file(const char *path);

/* Types */

typedef struct Type Type;
struct Type {
    int kind;
    Type *base;
    int array_len;
    int size;
    int align;
    Type *return_ty;
    Type *params;
    int is_func;
    struct Member *members;
    int is_incomplete;
    char *tag;
};

enum {
    TY_INT,
    TY_CHAR,
    TY_VOID,
    TY_PTR,
    TY_ARRAY,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION
};

Type *ty_int(void);
Type *ty_char(void);
Type *ty_void(void);
Type *pointer_to(Type *base);
Type *array_of(Type *base, int len);
Type *func_type(Type *ret, Type *params);
int is_integer(Type *ty);
Type *struct_type(int is_union, char *tag, struct Member *members);
struct Member *find_member(Type *ty, const char *name);

/* AST */

typedef struct Node Node;

typedef struct Obj Obj;
typedef struct Member Member;

struct Member {
    Member *next;
    char *name;
    Type *ty;
    int offset;
};

struct Obj {
    Obj *next;
    char *name;
    Type *ty;
    int is_local;
    int is_param;
    int is_enum_const;
    int is_static;
    int is_extern;
    int offset;
    int is_function;
    int has_sret;
    char *asm_name;
    Node *body;
    Obj *params;
    Obj *locals;
    int stack_size;
    char *init_data;
    int init_len;
    long enum_val;
};

struct Node {
    int kind;
    Node *next;
    Type *ty;

    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Node *args;
    Node *case_next;
    Node *default_case;
    Type *cast_ty;

    Obj *var;
    Member *member;
    char *member_name;
    long val;
    int case_label;
};

enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_NEG,
    ND_BITAND,
    ND_BITOR,
    ND_BITXOR,
    ND_SHL,
    ND_SHR,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_BITNOT,
    ND_SIZEOF,
    ND_CAST,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_NUM,
    ND_VAR,
    ND_MEMBER,
    ND_ASSIGN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EXPR_STMT,
    ND_LOGAND,
    ND_LOGOR,
    ND_NOT,
    ND_TERNARY,
    ND_NULL
};

Node *new_node(int kind);
Node *new_binary(int kind, Node *lhs, Node *rhs);
Node *new_unary(int kind, Node *expr);
Node *new_num(long val);
Node *new_var_node(Obj *var);
Node *new_block(Node *stmts);
Node *new_expr_stmt(Node *expr);

/* Parser */

Obj *parse(Token *tok);

/* Semantic */

void sema(Obj *prog);

/* Codegen */

void codegen(Obj *prog, FILE *out);

/* Utilities */

void *xcalloc(size_t nmemb, size_t size);
char *strndup2(const char *p, int len);
int startswith(const char *p, const char *q);




static char *read_all_stdin(void) {
    size_t cap = 4096;
    size_t len = 0;
    char *buf = xcalloc(1, cap);
    int c;

    while ((c = getchar()) != -1) {
        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = (char)c;
    }
    if (len == 0 || buf[len - 1] != '\n') {
        if (len + 2 >= cap) {
            cap = cap * 2 + 64;
            buf = realloc(buf, cap);
        }
        buf[len++] = '\n';
    }
    buf[len] = '\0';
    return buf;
}

static Obj *append_prog(Obj *all, Obj *prog) {
    Obj *tail;
    if (!all) return prog;
    tail = all;
    while (tail->next) tail = tail->next;
    tail->next = prog;
    return all;
}

static void parse_segment(char *start, char *end, int file_id, Obj **all) {
    char *buf;
    char *p;
    char *prefix;
    Token *tok;
    Obj *prog;

    if (!start || start >= end) return;

    buf = xcalloc(1, (size_t)(end - start + 2));
    p = buf;
    while (start < end) *p++ = *start++;
    *p = '\0';

    prefix = xcalloc(1, 32);
    sprintf(prefix, "F%d_", file_id);
    set_file_prefix(prefix);

    tok = tokenize(buf);
    prog = parse(tok);
    *all = append_prog(*all, prog);
}

int main(int argc, char **argv) {
    char *input;
    char *p;
    char *line;
    char *seg_start = NULL;
    Obj *all = NULL;
    int file_id = -1;

    (void)argc;
    (void)argv;

    input = read_all_stdin();

    line = input;
    p = input;
    while (*p) {
        if (*p == '\n') {
            *p = '\0';
            if (startswith(line, "//--FILE:")) {
                if (file_id >= 0 && seg_start) {
                    parse_segment(seg_start, line - 1, file_id, &all);
                }
                file_id++;
                seg_start = p + 1;
            }
            *p = '\n';
            p++;
            line = p;
            continue;
        }
        p++;
    }

    if (file_id >= 0 && seg_start) {
        parse_segment(seg_start, p, file_id, &all);
    } else {
        /* No markers: treat whole input as a single file. */
        file_id = 0;
        parse_segment(input, p, file_id, &all);
    }

    sema(all);
    codegen(all, stdout);
    return 0;
}

