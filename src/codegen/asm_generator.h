#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "../parser/ast.h"
#include <stdio.h>

#define MAX_LOCALS 128 // Define MAX_LOCALS or adjust as needed
typedef struct LocalVar {
    const char* name;
    int offset;
} LocalVar;
typedef struct {
    FILE* output;
    int temp_counter;
    int label_counter;
    int has_return_value; // Track if __return_value is assigned
    int emitted_sections; // Track if .data/.text emitted
    LocalVar locals[MAX_LOCALS]; // Add locals array to AsmGenerator
    int local_count;             // Track number of locals
} AsmGenerator;

// Create and destroy generator
AsmGenerator* create_asm_generator(FILE* output);
void free_asm_generator(AsmGenerator* generator);

// Main code generation function
void generate_assembly(AsmGenerator* generator, ASTNode* ast);

#endif // ASM_GENERATOR_H
