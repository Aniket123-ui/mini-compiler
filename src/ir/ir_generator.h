#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "../parser/parser.h"
#include <stdio.h>

char* new_temp();
char* new_label();
void generate_ir(ASTNode* node, FILE* output);

#endif // IR_GENERATOR_H
