#ifndef PDP11_C89_COMPILER_H
#define PDP11_C89_COMPILER_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

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
void set_current_filename(const char *name);
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

#endif
