#!/bin/sh
set -e

OUT=${1:-tests/sim_paths.txt}
shift || true

if [ $# -eq 0 ]; then
  # default compiler sources
  set -- src/main_sim.c src/tokenize.c src/parser.c src/type.c src/ast.c src/sema.c src/codegen.c src/util.c src/preprocess.c src/sim_support.c
fi

{
  for f in "$@"; do
    echo "PATH $f"
  done
} > "$OUT"

echo "Wrote $OUT"
