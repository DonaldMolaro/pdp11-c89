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

.PHONY: clean stage-2-smoke
