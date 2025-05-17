#include "ir_generator.h"
#include "../parser/ast.h"
#include "../utils/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int temp_var_counter = 0;
static int label_counter = 0;

char* new_temp() {
    char* temp = malloc(16);
    sprintf(temp, "t%d", ++temp_var_counter);
    return temp;
}

char* new_label() {
    char* label = malloc(16);
    sprintf(label, "L%d", ++label_counter);
    return label;
}

void generate_ir(ASTNode* node, FILE* output) {
    if (!node) return;

    switch (node->type) {

        case AST_NUMBER: {
            char* temp = new_temp();
            fprintf(output, "%s = %d\n", temp, node->number.value);
            // Store temp_var in a dedicated field if you have one; if not, you can consider adding one
            // For now, you can keep track externally or add a temp_var pointer to ASTNode struct
            node->temp_var = temp; // Make sure ASTNode struct has a `char *temp_var;` field
            break;
        }

        case AST_BINOP: {
            generate_ir(node->binop.left, output);
            generate_ir(node->binop.right, output);

            char* temp = new_temp();
            const char* op;

            switch (node->binop.op_type) {
                case OP_ADD: op = "+"; break;
                case OP_SUB: op = "-"; break;
                case OP_MUL: op = "*"; break;
                case OP_DIV: op = "/"; break;
                default: op = "?"; break;
            }

            fprintf(output, "%s = %s %s %s\n", 
                temp, 
                node->binop.left->temp_var,
                op,
                node->binop.right->temp_var);

            node->temp_var = temp;
            break;
        }

        case AST_IF: {
            char* label_else = new_label();
            char* label_end = new_label();

            generate_ir(node->if_stmt.condition, output);
            fprintf(output, "if_false %s goto %s\n", node->if_stmt.condition->temp_var, label_else);

            generate_ir(node->if_stmt.then_branch, output);  // then block
            fprintf(output, "goto %s\n", label_end);

            fprintf(output, "%s:\n", label_else);
            if (node->if_stmt.else_branch) {
                generate_ir(node->if_stmt.else_branch, output);
            }
            fprintf(output, "%s:\n", label_end);
            break;
        }

        case AST_WHILE: {
            char* label_start = new_label();
            char* label_end = new_label();

            fprintf(output, "%s:\n", label_start);
            generate_ir(node->while_stmt.condition, output);
            fprintf(output, "if_false %s goto %s\n", node->while_stmt.condition->temp_var, label_end);

            generate_ir(node->while_stmt.body, output);
            fprintf(output, "goto %s\n", label_start);
            fprintf(output, "%s:\n", label_end);
            break;
        }

        default:
            fprintf(output, "// [IR] Unsupported AST node type: %d\n", node->type);
            break;
    }
}
