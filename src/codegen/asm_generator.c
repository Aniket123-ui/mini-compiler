#include "asm_generator.h"
#include "../parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper: lookup stack offset for a local variable
int lookup_local_offset(AsmGenerator* gen, const char* name);

static int next_label(AsmGenerator* gen) {
    return gen->label_counter++;
}
  
static void generate_expression(AsmGenerator* gen, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_NUMBER:
            fprintf(gen->output, "    mov rax, %d\n", node->value);
            break;

        case AST_IDENTIFIER: {
            int local_offset = lookup_local_offset(gen, node->identifier);
            if (local_offset != -9999) {
                fprintf(gen->output, "    mov rax, [rbp%+d]\n", local_offset);
            } else {
                fprintf(gen->output, "    mov rax, [rel %s]\n", node->identifier);
            }
            break;
        }

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

// Helper: allocate stack space for locals
static void emit_function_prologue(FILE* out, int local_count) {
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    if (local_count > 0) {
        fprintf(out, "    sub rsp, %d\n", local_count * 8);
    }
}
static void emit_function_epilogue(FILE* out, int local_count) {
    if (local_count > 0) {
        fprintf(out, "    add rsp, %d\n", local_count * 8);
    }
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Helper: lookup stack offset for a local variable
int lookup_local_offset(AsmGenerator* gen, const char* name) {
    for (int i = 0; i < gen->local_count; ++i) {
        if (strcmp(gen->locals[i].name, name) == 0) {
            return gen->locals[i].offset;
        }
    }
    return -9999; // Use -9999 for not found
}

// Main codegen entry for function
void generate_assembly(AsmGenerator* gen, ASTNode* ast) {
    if (!gen || !ast) return;
    // Emit .data and .text sections only at the start (AST_FUNCTION)
    // Count locals and fill gen->locals
    gen->local_count = 0;
    if (ast->type == AST_FUNCTION) {
        if (ast->function.body && (ast->function.body->type == AST_BLOCK || ast->function.body->type == AST_COMPOUND)) {
            for (size_t i = 0; i < ast->function.body->block.count; ++i) {
                ASTNode* stmt = ast->function.body->block.statements[i];
                if (stmt && stmt->type == AST_DECLARATION) {
                    gen->locals[gen->local_count].name = stmt->declaration.name;
                    gen->locals[gen->local_count].offset = -(gen->local_count + 1) * 8;
                    gen->local_count++;
                }
            }
        }
        emit_function_prologue(gen->output, gen->local_count);
        for (size_t i = 0; i < ast->function.body->block.count; ++i) {
            ASTNode* stmt = ast->function.body->block.statements[i];
            if (stmt) {
                generate_assembly(gen, stmt);
            }
        }
        // At the end of main, print __return_value if it was assigned
        // Only emit print and epilogue/ret ONCE, and only after all code
        if (gen->has_return_value) {
            fprintf(gen->output, "    mov rdx, qword [rel __return_value]\n");
            fprintf(gen->output, "    lea rcx, [rel fmt]\n");
            fprintf(gen->output, "    call printf\n");
        }
        if (gen->local_count > 0) {
            fprintf(gen->output, "    add rsp, %d\n", gen->local_count * 8);
        }
        fprintf(gen->output, "    pop rbp\n");
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
            int local_offset = lookup_local_offset(gen, ast->declaration.name);
            if (ast->declaration.init) {
                generate_expression(gen, ast->declaration.init);
                if (local_offset != -9999) {
                    fprintf(gen->output, "    mov [rbp%+d], rax\n", local_offset);
                } else {
                    fprintf(gen->output, "    mov [rel %s], rax\n", ast->declaration.name);
                }
            } else {
                if (local_offset != -9999) {
                    fprintf(gen->output, "    mov [rbp%+d], 0\n", local_offset);
                } else {
                    fprintf(gen->output, "    mov [rel %s], 0\n", ast->declaration.name);
                }
            }
            break;
        case AST_RETURN:
            fprintf(stderr, "[DEBUG] generate_assembly: RETURN\n");
            generate_expression(gen, ast->return_stmt.expr);
            // Save result to __return_value for printing, but DO NOT emit epilogue/ret here
fprintf(gen->output, "    mov [rel __return_value], rax\n");

            gen->has_return_value = 1;
            break;

        case AST_ASSIGNMENT:
            fprintf(stderr, "[DEBUG] generate_assembly: ASSIGNMENT %s\n", ast->assignment.name);
            generate_expression(gen, ast->assignment.value);
            local_offset = lookup_local_offset(gen, ast->assignment.name);
            if (local_offset != -9999) {
                fprintf(gen->output, "    mov [rbp%+d], rax\n", local_offset);
            } else {
                fprintf(gen->output, "    mov [rel %s], rax\n", ast->assignment.name);
            }
            // Track if __return_value is assigned
            if (strcmp(ast->assignment.name, "__return_value") == 0) {
                gen->has_return_value = 1;
            }
            break;

        case AST_COMPOUND:
            fprintf(stderr, "[DEBUG] generate_assembly: COMPOUND with %zu statements\n", ast->block.count);
            for (size_t i = 0; i < ast->block.count; ++i) {
                if (ast->block.statements[i]) {
                    generate_assembly(gen, ast->block.statements[i]);
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
    gen->has_return_value = 0; // Initialize the flag
    gen->emitted_sections = 0; // Initialize emitted_sections flag
    return gen;
}

void free_asm_generator(AsmGenerator* gen) {
    if (gen) free(gen);
}
