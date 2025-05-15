CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall -g

OBJS = lex.yy.o parser.tab.o ast.o symbol_table.o ir_generator.o optimizer.o error_handler.o

compiler: $(OBJS)
	$(CC) $(CFLAGS) -o compiler $(OBJS)

lex.yy.c: lexer.l parser.tab.h
	$(FLEX) lexer.l

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f compiler $(OBJS) lex.yy.c parser.tab.c parser.tab.h

.PHONY: clean