#include "asm_generator.h"
#include <stdlib.h>

static void generate_expression(AsmGenerator* gen, ASTNode* node);
static char* new_label(AsmGenerator* gen);

AsmGenerator* create_asm_generator(FILE* output) {
    AsmGenerator* gen = malloc(sizeof(AsmGenerator));
    gen->output = output;
    gen->temp_counter = 0;
    gen->label_counter = 0;
    return gen;
}

static char* new_label(AsmGenerator* gen) {
    char* label = malloc(32);
    sprintf(label, "L%d", gen->label_counter++);
    return label;
}

static void generate_expression(AsmGenerator* gen, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            fprintf(gen->output, "    mov eax, %d\n", node->token->value);
            break;

        case AST_BINARY_OP:
            // Generate code for right operand first
            generate_expression(gen, node->right);
            // Save right operand result
            fprintf(gen->output, "    push eax\n");
            // Generate code for left operand
            generate_expression(gen, node->left);
            // Right operand is now in edx
            fprintf(gen->output, "    pop edx\n");

            switch (node->op_type) {
                case OP_PLUS:
                    fprintf(gen->output, "    add eax, edx\n");
                    break;
                case OP_MINUS:
                    fprintf(gen->output, "    sub eax, edx\n");
                    break;
                case OP_MULTIPLY:
                    fprintf(gen->output, "    imul eax, edx\n");
                    break;
                case OP_DIVIDE:
                    fprintf(gen->output, "    xor edx, edx\n");  // Clear edx for division
                    fprintf(gen->output, "    idiv edx\n");
                    break;
            }
            break;
    }
}

void generate_assembly(AsmGenerator* gen, ASTNode* ast) {
    // Write assembly header
    fprintf(gen->output, "section .text\n");
    fprintf(gen->output, "global _start\n\n");
    fprintf(gen->output, "_start:\n");

    // Generate code for the AST
    generate_expression(gen, ast);

    // Write exit syscall
    fprintf(gen->output, "    mov ebx, eax\n");  // Exit code = result
    fprintf(gen->output, "    mov eax, 1\n");    // syscall: exit
    fprintf(gen->output, "    int 0x80\n");
}

void free_asm_generator(AsmGenerator* gen) {
    if (gen) {
        free(gen);
    }
}