#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "../parser/ast.h"
#include <stdio.h>

// Generates a new temporary variable name like t1, t2, ...
char* new_temp();

// Generates a new label name like L1, L2, ...
char* new_label();

// Generates intermediate code from an AST and writes it to the output file
void generate_ir(ASTNode* node, FILE* output);

#endif // IR_GENERATOR_H
