#include "ir_generator.h"
#include "../parser/ast.h"
#include "../utils/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple IR instruction types
typedef enum {
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_LT,
    IR_GT,
    IR_LE,
    IR_GE,
    IR_EQ,
    IR_NEQ,
    IR_LOAD_CONST,
    IR_LOAD_VAR,
    IR_LABEL,
    IR_JUMP,
    IR_JUMP_IF_FALSE
} IRType;

// IR instruction struct
typedef struct IRInstruction {
    IRType type;
    char dest[32];
    char src1[32];
    char src2[32];
    int value; // for constants
    char label[32]; // for labels
    struct IRInstruction* next;
} IRInstruction;

static int temp_count = 0;
static int label_count = 0;
static void new_temp(char* buf) { sprintf(buf, "t%d", temp_count++); }
static void new_label(char* buf) { sprintf(buf, "L%d", label_count++); }

// Forward declaration
static IRInstruction* generate_ir_node(ASTNode* node, char* result);

// Main IR generation entry point
void generate_ir(ASTNode* node, FILE* output) {
    char result[32];
    temp_count = 0;
    label_count = 0;
    IRInstruction* ir = generate_ir_node(node, result);
    // Print IR
    fprintf(output, "[IR] Generated IR instructions:\n");
    for (IRInstruction* instr = ir; instr; instr = instr->next) {
        switch (instr->type) {
            case IR_ASSIGN:
                fprintf(output, "%s = %s\n", instr->dest, instr->src1);
                break;
            case IR_ADD:
                fprintf(output, "%s = %s + %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_SUB:
                fprintf(output, "%s = %s - %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_MUL:
                fprintf(output, "%s = %s * %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_DIV:
                fprintf(output, "%s = %s / %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_LT:
                fprintf(output, "%s = %s < %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_GT:
                fprintf(output, "%s = %s > %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_LE:
                fprintf(output, "%s = %s <= %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_GE:
                fprintf(output, "%s = %s >= %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_EQ:
                fprintf(output, "%s = %s == %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_NEQ:
                fprintf(output, "%s = %s != %s\n", instr->dest, instr->src1, instr->src2);
                break;
            case IR_LOAD_CONST:
                fprintf(output, "%s = %d\n", instr->dest, instr->value);
                break;
            case IR_LOAD_VAR:
                fprintf(output, "%s = %s\n", instr->dest, instr->src1);
                break;
            case IR_LABEL:
                fprintf(output, "%s:\n", instr->label);
                break;
            case IR_JUMP:
                fprintf(output, "goto %s\n", instr->label);
                break;
            case IR_JUMP_IF_FALSE:
                fprintf(output, "ifnot %s goto %s\n", instr->src1, instr->label);
                break;
        }
    }
    // Free IR list
    while (ir) {
        IRInstruction* next = ir->next;
        free(ir);
        ir = next;
    }
}

static IRInstruction* append_ir(IRInstruction* head, IRInstruction* tail) {
    if (!head) return tail;
    IRInstruction* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = tail;
    return head;
}

static IRInstruction* generate_ir_node(ASTNode* node, char* result) {
    if (!node) return NULL;
    switch (node->type) {
        case AST_NUMBER: {
            new_temp(result);
            IRInstruction* instr = calloc(1, sizeof(IRInstruction));
            instr->type = IR_LOAD_CONST;
            strcpy(instr->dest, result);
            instr->value = node->value;
            return instr;
        }
        case AST_IDENTIFIER: {
            new_temp(result);
            IRInstruction* instr = calloc(1, sizeof(IRInstruction));
            instr->type = IR_LOAD_VAR;
            strcpy(instr->dest, result);
            strcpy(instr->src1, node->identifier);
            return instr;
        }
        case AST_BINARY_OP: {
            char left[32], right[32];
            IRInstruction* left_ir = generate_ir_node(node->binop.left, left);
            IRInstruction* right_ir = generate_ir_node(node->binop.right, right);
            new_temp(result);
            IRInstruction* instr = calloc(1, sizeof(IRInstruction));
            switch (node->binop.op_type) {
                case OP_ADD: instr->type = IR_ADD; break;
                case OP_SUB: instr->type = IR_SUB; break;
                case OP_MUL: instr->type = IR_MUL; break;
                case OP_DIV: instr->type = IR_DIV; break;
                case OP_LT: instr->type = IR_LT; break;
                case OP_GT: instr->type = IR_GT; break;
                case OP_LE: instr->type = IR_LE; break;
                case OP_GE: instr->type = IR_GE; break;
                case OP_EQ: instr->type = IR_EQ; break;
                case OP_NEQ: instr->type = IR_NEQ; break;
                default: instr->type = IR_ADD; break;
            }
            strcpy(instr->dest, result);
            strcpy(instr->src1, left);
            strcpy(instr->src2, right);
            return append_ir(append_ir(left_ir, right_ir), instr);
        }
        case AST_ASSIGNMENT: {
            char rhs[32];
            IRInstruction* rhs_ir = generate_ir_node(node->assignment.value, rhs);
            IRInstruction* instr = calloc(1, sizeof(IRInstruction));
            instr->type = IR_ASSIGN;
            strcpy(instr->dest, node->assignment.name);
            strcpy(instr->src1, rhs);
            return append_ir(rhs_ir, instr);
        }
        case AST_COMPOUND: {
            IRInstruction* head = NULL;
            for (size_t i = 0; i < node->compound.count; ++i) {
                char dummy[32];
                IRInstruction* stmt_ir = generate_ir_node(node->compound.statements[i], dummy);
                head = append_ir(head, stmt_ir);
            }
            return head;
        }
        case AST_IF: {
            char cond[32], label_else[32], label_end[32];
            IRInstruction* cond_ir = generate_ir_node(node->if_stmt.condition, cond);
            new_label(label_else);
            new_label(label_end);
            IRInstruction* jump_if_false = calloc(1, sizeof(IRInstruction));
            jump_if_false->type = IR_JUMP_IF_FALSE;
            strcpy(jump_if_false->src1, cond);
            strcpy(jump_if_false->label, label_else);
            IRInstruction* then_ir = generate_ir_node(node->if_stmt.then_branch, NULL);
            IRInstruction* jump_end = calloc(1, sizeof(IRInstruction));
            jump_end->type = IR_JUMP;
            strcpy(jump_end->label, label_end);
            IRInstruction* else_label = calloc(1, sizeof(IRInstruction));
            else_label->type = IR_LABEL;
            strcpy(else_label->label, label_else);
            IRInstruction* else_ir = NULL;
            if (node->if_stmt.else_branch) {
                else_ir = generate_ir_node(node->if_stmt.else_branch, NULL);
            }
            IRInstruction* end_label = calloc(1, sizeof(IRInstruction));
            end_label->type = IR_LABEL;
            strcpy(end_label->label, label_end);
            // Chain: cond_ir -> jump_if_false -> then_ir -> jump_end -> else_label -> else_ir -> end_label
            IRInstruction* head = append_ir(cond_ir, jump_if_false);
            head = append_ir(head, then_ir);
            head = append_ir(head, jump_end);
            head = append_ir(head, else_label);
            head = append_ir(head, else_ir);
            head = append_ir(head, end_label);
            return head;
        }
        case AST_WHILE: {
            char cond[32], label_start[32], label_end[32];
            new_label(label_start);
            new_label(label_end);
            IRInstruction* start_label = calloc(1, sizeof(IRInstruction));
            start_label->type = IR_LABEL;
            strcpy(start_label->label, label_start);
            IRInstruction* cond_ir = generate_ir_node(node->while_stmt.condition, cond);
            IRInstruction* jump_if_false = calloc(1, sizeof(IRInstruction));
            jump_if_false->type = IR_JUMP_IF_FALSE;
            strcpy(jump_if_false->src1, cond);
            strcpy(jump_if_false->label, label_end);
            IRInstruction* body_ir = generate_ir_node(node->while_stmt.body, NULL);
            IRInstruction* jump_start = calloc(1, sizeof(IRInstruction));
            jump_start->type = IR_JUMP;
            strcpy(jump_start->label, label_start);
            IRInstruction* end_label = calloc(1, sizeof(IRInstruction));
            end_label->type = IR_LABEL;
            strcpy(end_label->label, label_end);
            // Chain: start_label -> cond_ir -> jump_if_false -> body_ir -> jump_start -> end_label
            IRInstruction* head = append_ir(start_label, cond_ir);
            head = append_ir(head, jump_if_false);
            head = append_ir(head, body_ir);
            head = append_ir(head, jump_start);
            head = append_ir(head, end_label);
            return head;
        }
        default:
            return NULL;
    }
}
