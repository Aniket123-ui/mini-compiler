#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

typedef struct {
    Parser* parser;
} Interpreter;

Interpreter* create_interpreter(Parser* parser);
int interpret(Interpreter* interpreter);
void free_interpreter(Interpreter* interpreter);

#endif