#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "../parser/ast.h"
#include <stdio.h>

typedef struct {
    FILE* output;
    int temp_counter;
    int label_counter;
} AsmGenerator;

// Create and destroy generator
AsmGenerator* create_asm_generator(FILE* output);
void free_asm_generator(AsmGenerator* generator);

// Main code generation function
void generate_assembly(AsmGenerator* generator, ASTNode* ast);

#endif // ASM_GENERATOR_H
