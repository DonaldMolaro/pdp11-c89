#!/bin/sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SIM="/Users/donaldmolaro/src/pdp-11/build/pdp11sim"

BOOT="$ROOT/tests/bootstrap.c"
ASM1="$ROOT/tests/pdp11cc_sim.asm"
ASM1_OUT="$ROOT/tests/pdp11cc_sim_out.asm"
ASM2_OUT="$ROOT/tests/pdp11cc_sim_out2.asm"

sh "$ROOT/tools/mk_bootstrap_input.sh" "$BOOT"

"$ROOT/pdp11cc" \
  "$ROOT/src/main_sim.c" \
  "$ROOT/src/tokenize.c" \
  "$ROOT/src/lex_literals.c" \
  "$ROOT/src/parser.c" \
  "$ROOT/src/parser_scope.c" \
  "$ROOT/src/type.c" \
  "$ROOT/src/ast.c" \
  "$ROOT/src/ast_build.c" \
  "$ROOT/src/sema.c" \
  "$ROOT/src/sema_type.c" \
  "$ROOT/src/sema_check.c" \
  "$ROOT/src/sema_const.c" \
  "$ROOT/src/codegen.c" \
  "$ROOT/src/codegen_expr.c" \
  "$ROOT/src/codegen_stmt.c" \
  "$ROOT/src/codegen_addr.c" \
  "$ROOT/src/codegen_functions.c" \
  "$ROOT/src/codegen_globals.c" \
  "$ROOT/src/util.c" \
  "$ROOT/src/preprocess.c" \
  "$ROOT/src/emitter.c" \
  "$ROOT/src/emit_utils.c" \
  "$ROOT/src/runtime.c" \
  "$ROOT/src/runtime_startup.c" \
  "$ROOT/src/runtime_helpers.c" \
  "$ROOT/src/runtime_stdio.c" \
  "$ROOT/src/runtime_format.c" \
  "$ROOT/src/runtime_fileio.c" \
  "$ROOT/src/runtime_math.c" \
  "$ROOT/src/runtime_string.c" \
  "$ROOT/src/sim_support.c" \
  -o "$ASM1"

"$SIM" "$ASM1" < "$BOOT" | sed '/^HALT=/d;/^R[0-7]=/d;/^N=/d' > "$ASM1_OUT"
"$SIM" "$ASM1_OUT" < "$BOOT" | sed '/^HALT=/d;/^R[0-7]=/d;/^N=/d' > "$ASM2_OUT"

if diff -q "$ASM1_OUT" "$ASM2_OUT" >/dev/null 2>&1; then
  echo "Stage-2 check: OK (stage1_out == stage2_out)"
else
  echo "Stage-2 check: MISMATCH"
  echo "Diff saved."
  diff -u "$ASM1_OUT" "$ASM2_OUT" > "$ROOT/tests/stage2.diff" || true
  exit 1
fi
