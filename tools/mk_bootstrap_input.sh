#!/bin/sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT=${1:-"$ROOT/tests/bootstrap.c"}

# Build a header block without preprocessor lines.
HEADER_TMP="/tmp/pdp11cc_bootstrap_headers.$$"
{
  sed '/^#/d' "$ROOT/include/stdarg.h"
  echo
  sed '/^#/d' "$ROOT/include/stddef.h"
  echo
  sed '/^#/d' "$ROOT/include/stdio.h"
  echo
  sed '/^#/d' "$ROOT/include/stdlib.h"
  echo
  sed '/^#/d' "$ROOT/include/string.h"
  echo
  sed '/^#/d' "$ROOT/include/ctype.h"
  echo
  sed '/^#/d' "$ROOT/src/compiler.h"
  echo
} > "$HEADER_TMP"

{
  for f in "$ROOT"/src/*.c; do
    case "$f" in
      */main.c) continue ;;
      */preprocess.c) continue ;;
      */preprocess_sim.c) continue ;;
    esac
    echo "//--FILE:$f"
    cat "$HEADER_TMP"
    sed '/^#/d' "$f"
    echo
  done
  # Add simulator entrypoint last.
  echo "//--FILE:src/main_sim.c"
  cat "$HEADER_TMP"
  sed '/^#/d' "$ROOT/src/main_sim.c"
  echo
} > "$OUT"

rm -f "$HEADER_TMP"

echo "Wrote $OUT"
