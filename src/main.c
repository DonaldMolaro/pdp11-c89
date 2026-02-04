#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    char *output_path = NULL;
    char *input;
    Token *tok;
    Obj *prog;
    Obj *all = NULL;
    Obj *tail = NULL;
    FILE *outfp;
    char *inputs[128];
    int ninputs = 0;
    int i;
    int file_id = 0;

    if (argc < 2) {
        fprintf(stderr, "usage: pdp11cc <input.c> [more.c ...] -o <output.asm>\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "missing output path after -o\n");
                return 1;
            }
            output_path = argv[++i];
            continue;
        }
        inputs[ninputs++] = argv[i];
    }

    if (ninputs == 0) {
        fprintf(stderr, "no input files\n");
        return 1;
    }

    for (i = 0; i < ninputs; i++) {
        char *prefix = xcalloc(1, 32);
        sprintf(prefix, "F%d_", file_id++);
        set_file_prefix(prefix);

        input = preprocess_file(inputs[i]);
        if (!input) {
            fprintf(stderr, "cannot open %s\n", inputs[i]);
            return 1;
        }
        tok = tokenize(input);
        prog = parse(tok);

        if (!all) {
            all = prog;
            tail = prog;
        } else {
            tail->next = prog;
        }
        while (tail && tail->next) tail = tail->next;
    }

    sema(all);

    if (output_path) {
        outfp = fopen(output_path, "w");
        if (!outfp) {
            fprintf(stderr, "cannot open %s\n", output_path);
            return 1;
        }
    } else {
        outfp = stdout;
    }

    codegen(all, outfp);

    if (outfp != stdout) fclose(outfp);
    return 0;
}
