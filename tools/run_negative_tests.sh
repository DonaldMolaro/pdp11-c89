#!/bin/sh
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
CC="$ROOT/pdp11cc"
BAD="$ROOT/tests/negative/indirect_call_bad.c"
OUT="/tmp/pdp11cc_neg.asm"

printf "Running negative tests...\n"

if "$CC" "$BAD" -o "$OUT" >/tmp/pdp11cc_neg.out 2>/tmp/pdp11cc_neg.err; then
  echo "Expected failure for $BAD, but compilation succeeded."
  exit 1
fi

if grep -q "invalid function call" /tmp/pdp11cc_neg.err; then
  echo "negative: indirect_call_bad => OK"
  exit 0
fi

echo "Expected error message not found for $BAD."
cat /tmp/pdp11cc_neg.err
exit 1
