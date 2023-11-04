cc = gcc
flags = -lm

cxx: cxx.c lexer.c parser.c decl.c type.c expr.c ssa.c hash.c list.c gen.c memory.c error.c
	$(cc) $^ -o $@ $(flags)

.PHONY: rm cxx