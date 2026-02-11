# pdp11-c89

A small C89 compiler targeting the PDP-11 simulator in `/Users/donaldmolaro/src/pdp-11`.

## Build

```sh
make
```

## Run

```sh
./pdp11cc examples/hello.c -o examples/hello.asm
/Users/donaldmolaro/src/pdp-11/build/pdp11sim examples/hello.asm
```

## True In-Sim Self Host

The simulator has no filesystem, so the in-sim compiler reads a single combined
input from stdin and writes asm to stdout. Use the simulator bootstrap files:

1. Build a combined input on the host (adds per-file markers):
```sh
sh tools/mk_bootstrap_input.sh tests/bootstrap.c
```

2. Generate the in-sim compiler asm:
```sh
./pdp11cc tests/bootstrap.c -o tests/pdp11cc_sim.asm
```

3. Run the compiler inside the simulator (stdin -> stdout):
```sh
/Users/donaldmolaro/src/pdp-11/build/pdp11sim tests/pdp11cc_sim.asm < tests/bootstrap.c > tests/pdp11cc_sim_out.asm
```

## Stage-2 Check

Run a stage-2 self-check by compiling the compiler twice in the simulator and
diffing the outputs:

```sh
sh tools/run_stage2_check.sh
```

Notes:
- `src/main_sim.c` splits the marked input into separate translation units.
- `src/sim_support.c` provides minimal libc + allocator + printf formatting.
- The helper script embeds headers and strips preprocessor lines.

## File-Based In-Sim Bootstrap (PATH Protocol)

With file I/O TRAPs (`#20-#25`) available, the in-sim compiler can read files
directly. Provide a PATH list on stdin:

```sh
make sim-paths
/Users/donaldmolaro/src/pdp-11/build/pdp11sim tests/pdp11cc_sim.asm < tests/sim_paths.txt > tests/pdp11cc_sim_paths_out.asm
```

The PATH list looks like:
```
PATH src/main_sim.c
PATH src/tokenize.c
...
```

## Banked Data (Simulator TRAP #26)

The compiler startup now uses the simulator's banked data mode:
- Copies the global data segment from bank 0 to bank 1.
- Switches the data bank to 1 before calling `main`.

This separates code (always fetched from bank 0) from data (bank 1), which
helps large in-sim builds fit by giving a full 64K data bank alongside the
64K code bank.

You can switch banks explicitly from C using:
```
pdp11_set_bank(int bank);
```

## Notes

- Output is PDP-11 assembly for the simulator's built-in assembler.
- The compiler currently targets a compact, K&R-style subset of C89.

## Source Layout

- `src/codegen.c`: Codegen entrypoint/orchestration.
- `src/codegen_functions.c`: Function emission.
- `src/codegen_globals.c`: Global/data emission.
- `src/codegen_expr.c`: Expression emission helpers used by `codegen.c`.
- `src/codegen_stmt.c`: Statement emission helpers used by `codegen.c`.
- `src/codegen_internal.h`: Shared codegen context and internal APIs.
- `src/runtime.c`: Runtime dispatcher.
- `src/runtime_startup.c`: Startup/data emission.
- `src/runtime_helpers.c`: Shared runtime helpers for banked data access.
- `src/runtime_stdio.c`: stdio runtime helpers (TRAP-backed).
- `src/runtime_format.c`: printf/scanf helpers.
- `src/runtime_fileio.c`: file I/O helpers and PDP-11 TRAPs.
- `src/runtime_math.c`: builtin math/bitwise helpers.
- `src/emitter.c`: Shared `emitln()` and output setup.
- `src/emit_utils.c`: Shared assembly emit helpers (stack push/pop).

### Banked Data Safety

Runtime globals (`__rt_*`) are accessed via register-deferred loads/stores to
ensure data comes from the active data bank (bank 1), not from code bank 0.

## Supported (current)

- Types: `int`, `char`, `void`, pointers, fixed-size arrays, `struct`/`union`/`enum`, function declarations/definitions.
- Statements: blocks, `if`/`else`, `while`, `for` (with declarations), `switch`/`case`/`default`, `break`, `continue`, `return`, declarations, expression statements.
- Expressions: literals, variables, assignment (including struct/union), `+ - * / %`, `== != < <= > >=`, `&& || !`, bitwise `& | ^ ~`, shifts `<< >>`, unary `& * -`, `sizeof`.
- Builtins: `putchar`, `getchar`, `puts` (via PDP-11 TRAPs).

## Calling Convention

- `R6` is stack pointer, stack grows downward.
- `JSR R5, func` used for calls; `RTS R5` for returns.
- Arguments pushed right-to-left; first arg at `4(R4)`.
- Return value in `R0`.
- Struct/union return uses an implicit first argument (sret pointer).
