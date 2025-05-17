#include "asm_generator.h"
#include "../parser/ast.h"
#include <stdio.h>
#include <stdlib.h>

static void generate_expression(AsmGenerator* gen, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            fprintf(gen->output, "    mov eax, %d\n", node->value);
            break;

        case AST_IDENTIFIER:
            fprintf(gen->output, "    mov eax, [%s]\n", node->identifier);
            break;

        case AST_BINARY_OP: {
            generate_expression(gen, node->binop.left);
            fprintf(gen->output, "    push eax\n");
            generate_expression(gen, node->binop.right);
            fprintf(gen->output, "    mov ebx, eax\n");
            fprintf(gen->output, "    pop eax\n");

            switch (node->binop.op_type) {
                case OP_ADD:
                    fprintf(gen->output, "    add eax, ebx\n");
                    break;
                case OP_SUB:
                    fprintf(gen->output, "    sub eax, ebx\n");
                    break;
                case OP_MUL:
                    fprintf(gen->output, "    imul eax, ebx\n");
                    break;
                case OP_DIV:
                    fprintf(gen->output, "    cdq\n");
                    fprintf(gen->output, "    idiv ebx\n");
                    break;
            }
            break;
        }

        default:
            break;
    }
}

void generate_assembly(AsmGenerator* gen, ASTNode* ast) {
    if (!gen || !ast) return;

    switch (ast->type) {
        case AST_NUMBER:
        case AST_IDENTIFIER:
        case AST_BINARY_OP:
            generate_expression(gen, ast);
            break;

        case AST_DECLARATION:
            fprintf(gen->output, "    ; declare %s\n", ast->declaration.name);
            if (ast->declaration.init) {
                generate_expression(gen, ast->declaration.init);
                fprintf(gen->output, "    mov [%s], eax\n", ast->declaration.name);
            }
            break;

        case AST_ASSIGNMENT:
            generate_expression(gen, ast->assignment.value);
            fprintf(gen->output, "    mov [%s], eax\n", ast->assignment.name);
            break;

        case AST_COMPOUND:
            for (size_t i = 0; i < ast->compound.count; ++i) {
                generate_assembly(gen, ast->compound.statements[i]);
            }
            break;

        default:
            break;
    }
}

AsmGenerator* create_asm_generator(FILE* output) {
    AsmGenerator* gen = malloc(sizeof(AsmGenerator));
    if (!gen) return NULL;
    gen->output = output;
    gen->temp_counter = 0;
    gen->label_counter = 0;
    return gen;
}

void free_asm_generator(AsmGenerator* gen) {
    if (gen) free(gen);
}
