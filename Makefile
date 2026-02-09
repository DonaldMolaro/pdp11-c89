CC = cc
CFLAGS = -std=c89 -Wall -Wextra -O2

SRC = \
	src/main.c \
	src/tokenize.c \
	src/parser.c \
	src/type.c \
	src/ast.c \
	src/sema.c \
	src/codegen.c \
	src/util.c \
	src/preprocess.c

OBJ = $(SRC:.c=.o)

pdp11cc: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

$(OBJ): src/compiler.h

clean:
	rm -f $(OBJ) pdp11cc

stage-2-smoke: pdp11cc
	@echo "Running stage-2 smoke check..."
	@echo "This will build bootstrap input, compile sim compiler, run two sim passes, and diff outputs."
	sh -x tools/run_stage2_check.sh

examples: pdp11cc
	@echo "Running examples on PDP-11 sim..."
	@for f in examples/*.c; do \
		base=$$(basename $$f .c); \
		./pdp11cc $$f -o examples/$$base.asm; \
		echo "=== $$base ==="; \
		/Users/donaldmolaro/src/pdp-11/build/pdp11sim examples/$$base.asm | sed '/^HALT=/d;/^R[0-7]=/d;/^N=/d'; \
		echo; \
	done

sim-input:
	@echo "Building sim input file..."
	@OUT=$${OUT:-tests/sim_input.txt}; \
	FILES=$${FILES:-src/main_sim.c src/tokenize.c src/parser.c src/type.c src/ast.c src/sema.c src/codegen.c src/util.c src/sim_support.c}; \
	tools/mk_sim_input.sh $$OUT $$FILES

.PHONY: clean stage-2-smoke examples sim-input
