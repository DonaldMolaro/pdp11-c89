#!/bin/sh
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
CC="$ROOT/pdp11cc"

printf "Running compile-only tests...\n"

for f in "$ROOT"/tests/cases/ok/*.c; do
  [ -e "$f" ] || continue
  out="/tmp/pdp11cc_$(basename "$f" .c).asm"
  if ! "$CC" "$f" -o "$out" >/tmp/pdp11cc_test.out 2>/tmp/pdp11cc_test.err; then
    echo "FAILED: expected success for $f"
    cat /tmp/pdp11cc_test.err
    exit 1
  fi
  echo "ok: $(basename "$f")"
  rm -f "$out"
 done

for f in "$ROOT"/tests/cases/err/*.c; do
  [ -e "$f" ] || continue
  base=$(basename "$f" .c)
  exp="$ROOT/tests/cases/err/$base.err"
  if "$CC" "$f" -o /tmp/pdp11cc_bad.asm >/tmp/pdp11cc_test.out 2>/tmp/pdp11cc_test.err; then
    echo "FAILED: expected failure for $f"
    exit 1
  fi
  if [ -f "$exp" ]; then
    if grep -q -F -f "$exp" /tmp/pdp11cc_test.err; then
      echo "err: $base"
    else
      echo "FAILED: error output mismatch for $f"
      echo "Expected pattern(s):"
      cat "$exp"
      echo "Actual:"
      cat /tmp/pdp11cc_test.err
      exit 1
    fi
  else
    echo "err: $base (no expected pattern)"
  fi
 done

printf "All compile-only tests passed.\n"
