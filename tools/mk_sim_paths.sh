#!/bin/sh
set -e

OUT=${1:-tests/sim_paths.txt}
shift || true

if [ $# -eq 0 ]; then
  # default compiler sources
  set -- src/main_sim.c src/tokenize.c src/lex_literals.c src/parser.c src/parser_scope.c src/type.c src/ast.c src/ast_build.c src/sema.c src/sema_type.c src/sema_check.c src/sema_const.c src/codegen.c src/codegen_expr.c src/codegen_stmt.c src/codegen_addr.c src/codegen_functions.c src/codegen_globals.c src/util.c src/preprocess.c src/emitter.c src/emit_utils.c src/runtime.c src/runtime_startup.c src/runtime_helpers.c src/runtime_stdio.c src/runtime_format.c src/runtime_fileio.c src/runtime_math.c src/runtime_string.c src/sim_support.c
fi

{
  for f in "$@"; do
    echo "PATH $f"
  done
} > "$OUT"

echo "Wrote $OUT"
