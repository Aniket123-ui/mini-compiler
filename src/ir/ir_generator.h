#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../parser/ast.h"   // Use ast.h because generate_ir works on ASTNode*
#include <stdio.h>

char* new_temp();
char* new_label();
void generate_ir(ASTNode* node, FILE* output);

#ifdef __cplusplus
}
#endif

#endif // IR_GENERATOR_H
