#include "compiler.h"

#include <stddef.h>
#include <stdlib.h>

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

static void parse_file_path(const char *path, int file_id, Obj **all) {
    char *prefix = xcalloc(1, 32);
    char *input;
    Token *tok;
    Obj *prog;

    sprintf(prefix, "F%d_", file_id);
    set_file_prefix(prefix);
    set_current_filename(path);

    input = preprocess_file(path);
    if (!input) {
        fprintf(stderr, "cannot open %s\n", path);
        return;
    }
    tok = tokenize(input);
    prog = parse(tok);
    *all = append_prog(*all, prog);
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
    set_current_filename("<segment>");

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
    int in_file_block = 0;

    (void)argc;
    (void)argv;

    input = read_all_stdin();

    line = input;
    p = input;
    while (*p) {
        if (*p == '\n') {
            *p = '\0';
            if (startswith(line, "PATH ")) {
                char *path = line + 5;
                if (seg_start && in_file_block) {
                    parse_segment(seg_start, line - 1, file_id, &all);
                    seg_start = NULL;
                    in_file_block = 0;
                }
                file_id++;
                parse_file_path(path, file_id, &all);
            } else if (startswith(line, "//--FILE:") || startswith(line, "FILE ")) {
                if (file_id >= 0 && seg_start) {
                    parse_segment(seg_start, line - 1, file_id, &all);
                }
                file_id++;
                seg_start = p + 1;
                in_file_block = startswith(line, "FILE ");
            } else if (in_file_block && startswith(line, "END")) {
                if (seg_start) {
                    parse_segment(seg_start, line - 1, file_id, &all);
                }
                seg_start = NULL;
                in_file_block = 0;
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
