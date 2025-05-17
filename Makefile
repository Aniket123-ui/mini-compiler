CC = gcc
CFLAGS = -Wall -g
OBJS = main.o lexer.o parser.o ir_generator.o error_handler.o interpreter.o

all: mini_compiler

mini_compiler: $(OBJS)
	$(CC) $(CFLAGS) -o mini_compiler $(OBJS)

clean:
	rm -f *.o mini_compiler
