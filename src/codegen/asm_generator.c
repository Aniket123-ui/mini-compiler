#include "asm_generator.h"
#include "../parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int next_label(AsmGenerator* gen) {
    return gen->label_counter++;
}

static void generate_expression(AsmGenerator* gen, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            fprintf(gen->output, "    mov rax, %d\n", node->value);
            break;

        case AST_IDENTIFIER:
            fprintf(gen->output, "    mov rax, [%s]\n", node->identifier);
            break;

        case AST_BINARY_OP: {
            generate_expression(gen, node->binop.left);
            fprintf(gen->output, "    push rax\n");
            generate_expression(gen, node->binop.right);
            fprintf(gen->output, "    mov rbx, rax\n");
            fprintf(gen->output, "    pop rax\n");

            switch (node->binop.op_type) {
                case OP_ADD:
                    fprintf(gen->output, "    add rax, rbx\n");
                    break;
                case OP_SUB:
                    fprintf(gen->output, "    sub rax, rbx\n");
                    break;
                case OP_MUL:
                    fprintf(gen->output, "    imul rax, rbx\n");
                    break;
                case OP_DIV:
                    fprintf(gen->output, "    cqo\n");
                    fprintf(gen->output, "    idiv rbx\n");
                    break;
                case OP_LT:
                case OP_GT:
                case OP_LE:
                case OP_GE:
                case OP_EQ:
                case OP_NEQ: {
                    // Comparison: rax = left, rbx = right
                    fprintf(gen->output, "    cmp rax, rbx\n");
                    const char* set_instr = NULL;
                    switch (node->binop.op_type) {
                        case OP_LT: set_instr = "setl"; break;
                        case OP_GT: set_instr = "setg"; break;
                        case OP_LE: set_instr = "setle"; break;
                        case OP_GE: set_instr = "setge"; break;
                        case OP_EQ: set_instr = "sete"; break;
                        case OP_NEQ: set_instr = "setne"; break;
                        default: set_instr = "sete"; break;
                    }
                    fprintf(gen->output, "    %s al\n", set_instr);
                    fprintf(gen->output, "    movzx rax, al\n");
                    break;
                }
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
}

void generate_assembly(AsmGenerator* gen, ASTNode* ast) {
    if (!gen || !ast) {
        fprintf(stderr, "[DEBUG] generate_assembly: NULL gen or ast\n");
        return;
    }
    fprintf(stderr, "[DEBUG] generate_assembly: node type %d\n", ast->type);
    switch (ast->type) {
        case AST_NUMBER:
        case AST_IDENTIFIER:
        case AST_BINARY_OP:
            generate_expression(gen, ast);
            break;

        case AST_DECLARATION:
            fprintf(stderr, "[DEBUG] generate_assembly: DECLARATION %s\n", ast->declaration.name);
            fprintf(gen->output, "    ; declare %s\n", ast->declaration.name);
            if (ast->declaration.init) {
                generate_expression(gen, ast->declaration.init);
                fprintf(gen->output, "    mov [%s], rax\n", ast->declaration.name);
            } else {
                // Emit default value for uninitialized vars
                fprintf(gen->output, "    mov [%s], 0\n", ast->declaration.name);
            }
            break;

        case AST_ASSIGNMENT:
            fprintf(stderr, "[DEBUG] generate_assembly: ASSIGNMENT %s\n", ast->assignment.name);
            generate_expression(gen, ast->assignment.value);
            fprintf(gen->output, "    mov [%s], rax\n", ast->assignment.name);
            break;

        case AST_COMPOUND:
            fprintf(stderr, "[DEBUG] generate_assembly: COMPOUND with %zu statements\n", ast->compound.count);
            for (size_t i = 0; i < ast->compound.count; ++i) {
                if (ast->compound.statements[i]) {
                    generate_assembly(gen, ast->compound.statements[i]);
                } else {
                    fprintf(stderr, "[DEBUG] generate_assembly: NULL child in COMPOUND at %zu\n", i);
                }
            }
            break;

        case AST_IF: {
            fprintf(stderr, "[DEBUG] generate_assembly: IF\n");
            int label_else = next_label(gen);
            int label_end = next_label(gen);
            generate_expression(gen, ast->if_stmt.condition);
            fprintf(gen->output, "    cmp rax, 0\n");
            fprintf(gen->output, "    je .L%d\n", label_else);
            generate_assembly(gen, ast->if_stmt.then_branch);
            fprintf(gen->output, "    jmp .L%d\n", label_end);
            fprintf(gen->output, ".L%d:\n", label_else);
            if (ast->if_stmt.else_branch) {
                generate_assembly(gen, ast->if_stmt.else_branch);
            }
            fprintf(gen->output, ".L%d:\n", label_end);
            break;
        }
        case AST_WHILE: {
            fprintf(stderr, "[DEBUG] generate_assembly: WHILE\n");
            int label_start = next_label(gen);
            int label_end = next_label(gen);
            fprintf(gen->output, ".L%d:\n", label_start);
            generate_expression(gen, ast->while_stmt.condition);
            fprintf(gen->output, "    cmp rax, 0\n");
            fprintf(gen->output, "    je .L%d\n", label_end);
            generate_assembly(gen, ast->while_stmt.body);
            fprintf(gen->output, "    jmp .L%d\n", label_start);
            fprintf(gen->output, ".L%d:\n", label_end);
            break;
        }
        default:
            fprintf(stderr, "[WARNING] generate_assembly: Unknown AST node type %d\n", (int)ast->type);
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
