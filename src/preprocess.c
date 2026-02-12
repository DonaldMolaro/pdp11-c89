#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Macro Macro;
struct Macro {
    Macro *next;
    char *name;
    char *value;
    int is_func;
    int param_count;
    char *params[8];
};

static Macro *macros;

typedef struct Cond Cond;
struct Cond {
    int active;
    int matched;
    int seen_else;
};

static Macro *find_macro(const char *name, int len);
static int is_ident1(int c);
static int is_ident2(int c);
static const char *pp_skip_ws(const char *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

static void expand_macros_line(const char *line, char **out, size_t *len, size_t *cap);
static long pp_parse_number(const char *p, const char **endp);

static long pp_parse_expr_or(const char **pp);

static long pp_parse_primary(const char **pp) {
    const char *p = pp_skip_ws(*pp);
    long v = 0;

    if (*p == '(') {
        p++;
        v = pp_parse_expr_or(&p);
        p = pp_skip_ws(p);
        if (*p == ')') p++;
        *pp = p;
        return v;
    }

    if (strncmp(p, "defined", 7) == 0 && !is_ident2(p[7])) {
        p += 7;
        p = pp_skip_ws(p);
        if (*p == '(') {
            const char *start;
            int len = 0;
            p++;
            p = pp_skip_ws(p);
            start = p;
            if (is_ident1(*p)) {
                p++;
                while (is_ident2(*p)) p++;
                len = (int)(p - start);
            }
            p = pp_skip_ws(p);
            if (*p == ')') p++;
            *pp = p;
            return find_macro(start, len) ? 1 : 0;
        }
        if (is_ident1(*p)) {
            const char *start = p;
            int len;
            p++;
            while (is_ident2(*p)) p++;
            len = (int)(p - start);
            *pp = p;
            return find_macro(start, len) ? 1 : 0;
        }
        *pp = p;
        return 0;
    }

    if (is_ident1(*p)) {
        p++;
        while (is_ident2(*p)) p++;
        *pp = p;
        return 0;
    }

    v = pp_parse_number(p, &p);
    *pp = p;
    return v;
}

static long pp_parse_unary(const char **pp) {
    const char *p = pp_skip_ws(*pp);
    if (*p == '!') {
        p++;
        *pp = p;
        return !pp_parse_unary(pp);
    }
    return pp_parse_primary(pp);
}

static long pp_parse_expr_and(const char **pp) {
    long v = pp_parse_unary(pp);
    const char *p = pp_skip_ws(*pp);
    while (p[0] == '&' && p[1] == '&') {
        long rhs;
        p += 2;
        *pp = p;
        rhs = pp_parse_unary(pp);
        v = (v && rhs) ? 1 : 0;
        p = pp_skip_ws(*pp);
    }
    *pp = p;
    return v;
}

static long pp_parse_expr_or(const char **pp) {
    long v = pp_parse_expr_and(pp);
    const char *p = pp_skip_ws(*pp);
    while (p[0] == '|' && p[1] == '|') {
        long rhs;
        p += 2;
        *pp = p;
        rhs = pp_parse_expr_and(pp);
        v = (v || rhs) ? 1 : 0;
        p = pp_skip_ws(*pp);
    }
    *pp = p;
    return v;
}

static int eval_pp_expr(const char *expr) {
    char *expanded = NULL;
    size_t len = 0;
    size_t cap = 0;
    const char *p;
    long v;

    expand_macros_line(expr, &expanded, &len, &cap);
    if (!expanded) return 0;
    p = expanded;
    v = pp_parse_expr_or(&p);
    free(expanded);
    return v != 0;
}

static long pp_parse_number(const char *p, const char **endp) {
    long v = 0;
    int base = 10;
    const char *start = p;

    if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        base = 16;
        p += 2;
    }

    while (*p) {
        int d = -1;
        if (*p >= '0' && *p <= '9') {
            d = *p - '0';
        } else if (base == 16 && *p >= 'a' && *p <= 'f') {
            d = *p - 'a' + 10;
        } else if (base == 16 && *p >= 'A' && *p <= 'F') {
            d = *p - 'A' + 10;
        } else {
            break;
        }
        if (d >= base) break;
        v = v * base + d;
        p++;
    }

    if (p == start || (base == 16 && p == start + 2)) {
        *endp = start;
        return 0;
    }

    *endp = p;
    return v;
}

static int parent_active(Cond *stack, int depth) {
    int i;
    for (i = 0; i < depth; i++) {
        if (!stack[i].active) return 0;
    }
    return 1;
}

static char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    char *buf;
    long size;

    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = xcalloc(1, size + 2);
    fread(buf, 1, size, fp);
    if (size == 0 || buf[size - 1] != '\n') buf[size++] = '\n';
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

static Macro *find_macro(const char *name, int len) {
    Macro *m;
    for (m = macros; m; m = m->next) {
        if ((int)strlen(m->name) == len && strncmp(m->name, name, len) == 0)
            return m;
    }
    return NULL;
}

static char *xstrdup(const char *s) {
    size_t n = strlen(s);
    char *p = xcalloc(1, n + 1);
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static void clear_params(Macro *m) {
    int i;
    for (i = 0; i < m->param_count; i++) {
        free(m->params[i]);
        m->params[i] = NULL;
    }
    m->param_count = 0;
}

static void undef_macro(const char *name, int len) {
    Macro **pm = &macros;
    while (*pm) {
        Macro *m = *pm;
        if ((int)strlen(m->name) == len && strncmp(m->name, name, len) == 0) {
            *pm = m->next;
            if (m->value) free(m->value);
            clear_params(m);
            free(m->name);
            free(m);
            return;
        }
        pm = &m->next;
    }
}

static void define_macro(const char *name, int len, const char *value, int is_func, char **params, int param_count) {
    Macro *m = find_macro(name, len);
    if (!m) {
        m = xcalloc(1, sizeof(Macro));
        m->name = strndup2(name, len);
        m->next = macros;
        macros = m;
    }
    if (m->value) free(m->value);
    clear_params(m);
    m->value = value ? xstrdup(value) : xstrdup("");
    m->is_func = is_func;
    m->param_count = param_count;
    if (param_count > 0) {
        int i;
        for (i = 0; i < param_count; i++) {
            m->params[i] = xstrdup(params[i]);
        }
    }
}

static void clear_macros(void) {
    Macro *m = macros;
    while (m) {
        Macro *next = m->next;
        if (m->value) free(m->value);
        clear_params(m);
        free(m->name);
        free(m);
        m = next;
    }
    macros = NULL;
}

static int is_ident1(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int is_ident2(int c) {
    return is_ident1(c) || (c >= '0' && c <= '9');
}

static void append(char **buf, size_t *len, size_t *cap, const char *s) {
    size_t n = strlen(s);
    if (*len + n + 1 > *cap) {
        *cap = (*cap + n + 64) * 2;
        *buf = realloc(*buf, *cap);
    }
    memcpy(*buf + *len, s, n);
    *len += n;
    (*buf)[*len] = '\0';
}

static void append_n(char **buf, size_t *len, size_t *cap, const char *s, size_t n) {
    if (*len + n + 1 > *cap) {
        *cap = (*cap + n + 64) * 2;
        *buf = realloc(*buf, *cap);
    }
    memcpy(*buf + *len, s, n);
    *len += n;
    (*buf)[*len] = '\0';
}

static char *dir_of(const char *path) {
    const char *slash = strrchr(path, '/');
    if (!slash) return xstrdup(".");
    return strndup2(path, (int)(slash - path));
}

static char *join_path(const char *dir, const char *file) {
    size_t d = strlen(dir);
    size_t f = strlen(file);
    char *out = xcalloc(1, d + f + 2);
    memcpy(out, dir, d);
    out[d] = '/';
    memcpy(out + d + 1, file, f);
    out[d + 1 + f] = '\0';
    return out;
}

static char *preprocess_text(const char *text, const char *base_dir);

static char *resolve_include_path(const char *base_dir, const char *inc, int is_angle) {
    char *path = NULL;
    char *src = NULL;

    if (!is_angle) {
        path = join_path(base_dir, inc);
        src = read_file(path);
        if (src) { free(path); free(src); return join_path(base_dir, inc); }
        free(path);
    }

    {
        char *parent = dir_of(base_dir);
        char *incdir = join_path(parent, "include");
        path = join_path(incdir, inc);
        src = read_file(path);
        free(parent);
        free(incdir);
        if (src) { free(src); return path; }
        free(path);
    }

    path = join_path("./include", inc);
    src = read_file(path);
    if (src) { free(src); return path; }
    free(path);
    return NULL;
}

static char *handle_include(const char *line, const char *base_dir) {
    const char *p = line;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (*p != '#') return NULL;
    p++;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (strncmp(p, "include", 7) != 0) return NULL;
    p += 7;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (*p != '\"' && *p != '<') return NULL;
    {
        int is_angle = (*p == '<');
        char endch = is_angle ? '>' : '\"';
        p++;
        const char *start = p;
        while (*p && *p != endch) p++;
        if (*p != endch) return NULL;
        {
            char *inc = strndup2(start, (int)(p - start));
            char *path = resolve_include_path(base_dir, inc, is_angle);
            char *src = path ? read_file(path) : NULL;
            free(inc);
            if (!src) { if (path) free(path); return NULL; }
            {
                char *dir = dir_of(path);
                char *out = preprocess_text(src, dir);
                free(dir);
                free(src);
                free(path);
                return out;
            }
        }
    }
}

static void handle_define(const char *line) {
    const char *p = line;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (*p != '#') return;
    p++;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (strncmp(p, "define", 6) != 0) return;
    p += 6;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (!is_ident1(*p)) return;
    const char *name = p;
    p++;
    while (is_ident2(*p)) p++;
    {
        const char *end = p;
        int is_func = 0;
        char *params[8];
        int param_count = 0;
        if (*p == '(') {
            is_func = 1;
            p++;
            while (*p && *p != ')') {
                while (*p && (*p == ' ' || *p == '\t')) p++;
                if (*p == ')') break;
                if (param_count < 8) {
                    const char *ps = p;
                    while (is_ident2(*p)) p++;
                    params[param_count++] = strndup2(ps, (int)(p - ps));
                } else {
                    while (is_ident2(*p)) p++;
                }
                while (*p && (*p == ' ' || *p == '\t')) p++;
                if (*p == ',') p++;
            }
            if (*p == ')') p++;
        }
        while (*p && (*p == ' ' || *p == '\t')) p++;
        define_macro(name, (int)(end - name), p, is_func, params, param_count);
        if (param_count > 0) {
            int i;
            for (i = 0; i < param_count; i++) free(params[i]);
        }
    }
}

static void handle_undef(const char *line) {
    const char *p = line;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (*p != '#') return;
    p++;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (strncmp(p, "undef", 5) != 0) return;
    p += 5;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (!is_ident1(*p)) return;
    {
        const char *name = p;
        p++;
        while (is_ident2(*p)) p++;
        undef_macro(name, (int)(p - name));
    }
}

static void append_repl(const char *value, Macro *m, char **args, int argc, char **out, size_t *len, size_t *cap) {
    const char *p = value;
    while (*p) {
        if (is_ident1(*p)) {
            const char *start = p;
            p++;
            while (is_ident2(*p)) p++;
            {
                int i;
                int replaced = 0;
                for (i = 0; i < m->param_count; i++) {
                    if ((int)(p - start) == (int)strlen(m->params[i]) &&
                        strncmp(start, m->params[i], (size_t)(p - start)) == 0) {
                        if (i < argc) append(out, len, cap, args[i]);
                        replaced = 1;
                        break;
                    }
                }
                if (!replaced) append_n(out, len, cap, start, (size_t)(p - start));
            }
            continue;
        }
        append_n(out, len, cap, p, 1);
        p++;
    }
}

static char *dup_trim(const char *start, const char *end) {
    while (start < end && (*start == ' ' || *start == '\t')) start++;
    while (end > start && (end[-1] == ' ' || end[-1] == '\t')) end--;
    return strndup2(start, (int)(end - start));
}

static void expand_macros_line_depth(const char *line, int depth, char **out, size_t *len, size_t *cap);

static void expand_macros_line(const char *line, char **out, size_t *len, size_t *cap) {
    expand_macros_line_depth(line, 0, out, len, cap);
}

static void expand_macros_line_depth(const char *line, int depth, char **out, size_t *len, size_t *cap) {
    const char *p = line;
    if (depth > 16) {
        append(out, len, cap, line);
        return;
    }
    while (*p) {
        if (is_ident1(*p)) {
            const char *start = p;
            p++;
            while (is_ident2(*p)) p++;
            {
                Macro *m = find_macro(start, (int)(p - start));
                if (m && m->is_func && *p == '(') {
                    char *args[8];
                    int argc = 0;
                    int depth = 0;
                    const char *arg_start;
                    p++; /* skip '(' */
                    arg_start = p;
                    depth = 1;
                    while (*p && depth > 0) {
                        if (*p == '(') depth++;
                        else if (*p == ')') depth--;
                        if (depth == 1 && *p == ',') {
                            if (argc < 8) args[argc++] = dup_trim(arg_start, p);
                            p++;
                            while (*p == ' ' || *p == '\t') p++;
                            arg_start = p;
                            continue;
                        }
                        if (depth == 0) break;
                        p++;
                    }
                    if (depth == 0) {
                        if (argc < 8) args[argc++] = dup_trim(arg_start, p);
                        p++; /* skip ')' */
                    }
                    {
                        int i;
                        for (i = 0; i < argc; i++) {
                            char *tmp = NULL;
                            size_t tlen = 0;
                            size_t tcap = 0;
                            expand_macros_line_depth(args[i], depth + 1, &tmp, &tlen, &tcap);
                            free(args[i]);
                            args[i] = tmp ? tmp : xstrdup("");
                        }
                    }
                    append_repl(m->value, m, args, argc, out, len, cap);
                    {
                        int i;
                        for (i = 0; i < argc; i++) free(args[i]);
                    }
                } else if (m && !m->is_func) {
                    char *tmp = NULL;
                    size_t tlen = 0;
                    size_t tcap = 0;
                    expand_macros_line_depth(m->value, depth + 1, &tmp, &tlen, &tcap);
                    if (tmp) {
                        append(out, len, cap, tmp);
                        free(tmp);
                    }
                } else {
                    append_n(out, len, cap, start, (size_t)(p - start));
                }
            }
            continue;
        }
        append_n(out, len, cap, p, 1);
        p++;
    }
}

static char *preprocess_text(const char *text, const char *base_dir) {
    const char *p = text;
    size_t cap = 4096;
    size_t len = 0;
    char *out = xcalloc(1, cap);
    Cond cond_stack[32];
    int cond_depth = 0;

    while (*p) {
        const char *line = p;
        while (*p && *p != '\n') p++;
        {
            size_t linelen = (size_t)(p - line);
            char *linebuf = strndup2(line, (int)linelen);
            int active = parent_active(cond_stack, cond_depth);
            char *inc = handle_include(linebuf, base_dir);
            if (inc) {
                if (active) {
                    append(&out, &len, &cap, inc);
                    append(&out, &len, &cap, "\n");
                }
                free(inc);
                free(linebuf);
                if (*p == '\n') p++;
                continue;
            }
            handle_define(linebuf);
            handle_undef(linebuf);
            {
                const char *q = linebuf;
                while (*q && (*q == ' ' || *q == '\t')) q++;
                if (*q == '#') {
                    q++;
                    while (*q && (*q == ' ' || *q == '\t')) q++;
                    if (strncmp(q, "ifdef", 5) == 0) {
                        int parent = parent_active(cond_stack, cond_depth);
                        q += 5;
                        while (*q && (*q == ' ' || *q == '\t')) q++;
                        if (cond_depth < 32) {
                            Macro *m = find_macro(q, (int)strlen(q));
                            cond_stack[cond_depth].active = parent && (m != NULL);
                            cond_stack[cond_depth].matched = parent && (m != NULL);
                            cond_stack[cond_depth].seen_else = 0;
                            cond_depth++;
                        }
                    } else if (strncmp(q, "ifndef", 6) == 0) {
                        int parent = parent_active(cond_stack, cond_depth);
                        q += 6;
                        while (*q && (*q == ' ' || *q == '\t')) q++;
                        if (cond_depth < 32) {
                            Macro *m = find_macro(q, (int)strlen(q));
                            cond_stack[cond_depth].active = parent && (m == NULL);
                            cond_stack[cond_depth].matched = parent && (m == NULL);
                            cond_stack[cond_depth].seen_else = 0;
                            cond_depth++;
                        }
                    } else if (strncmp(q, "if", 2) == 0 && !is_ident2(q[2])) {
                        int parent = parent_active(cond_stack, cond_depth);
                        int cond = 0;
                        q += 2;
                        while (*q && (*q == ' ' || *q == '\t')) q++;
                        cond = eval_pp_expr(q);
                        if (cond_depth < 32) {
                            cond_stack[cond_depth].active = parent && cond;
                            cond_stack[cond_depth].matched = parent && cond;
                            cond_stack[cond_depth].seen_else = 0;
                            cond_depth++;
                        }
                    } else if (strncmp(q, "elif", 4) == 0 && !is_ident2(q[4])) {
                        if (cond_depth > 0 && !cond_stack[cond_depth - 1].seen_else) {
                            int parent = parent_active(cond_stack, cond_depth - 1);
                            int cond = 0;
                            q += 4;
                            while (*q && (*q == ' ' || *q == '\t')) q++;
                            if (!cond_stack[cond_depth - 1].matched) {
                                cond = eval_pp_expr(q);
                                cond_stack[cond_depth - 1].active = parent && cond;
                                if (cond_stack[cond_depth - 1].active) cond_stack[cond_depth - 1].matched = 1;
                            } else {
                                cond_stack[cond_depth - 1].active = 0;
                            }
                        }
                    } else if (strncmp(q, "else", 4) == 0) {
                        if (cond_depth > 0 && !cond_stack[cond_depth - 1].seen_else) {
                            int parent = parent_active(cond_stack, cond_depth - 1);
                            cond_stack[cond_depth - 1].active = parent && !cond_stack[cond_depth - 1].matched;
                            cond_stack[cond_depth - 1].matched = 1;
                            cond_stack[cond_depth - 1].seen_else = 1;
                        }
                    } else if (strncmp(q, "endif", 5) == 0) {
                        if (cond_depth > 0) cond_depth--;
                    }
                    free(linebuf);
                    if (*p == '\n') p++;
                    continue;
                }
            }
            if (!active) {
                free(linebuf);
                if (*p == '\n') p++;
                continue;
            }
            expand_macros_line(linebuf, &out, &len, &cap);
            append(&out, &len, &cap, "\n");
            free(linebuf);
        }
        if (*p == '\n') p++;
    }

    return out;
}

char *preprocess_file(const char *path) {
    clear_macros();
    char *src = read_file(path);
    if (!src) return NULL;
    {
        char *dir = dir_of(path);
        char *out = preprocess_text(src, dir);
        free(dir);
        free(src);
        return out;
    }
}
