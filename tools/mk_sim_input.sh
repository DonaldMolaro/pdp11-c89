#!/bin/sh
set -e

if [ $# -lt 2 ]; then
  echo "usage: $0 <output> <file1.c> [file2.c ...]" >&2
  exit 1
fi

OUT=$1
shift

{
  for f in "$@"; do
    echo "FILE $f"
    cat "$f"
    echo "END"
  done
} > "$OUT"

echo "Wrote $OUT"
