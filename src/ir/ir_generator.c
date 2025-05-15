#include "ir_generator.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

static int temp_var_counter = 0;
static int label_counter = 0;

char* new_temp() {
    char* temp = malloc(32);
    sprintf(temp, "t%d", ++temp_var_counter);
    return temp;
}

char* new_label() {
    char* label = malloc(32);
    sprintf(label, "L%d", ++label_counter);
    return label;
}

void generate_ir(ASTNode* node, FILE* output) {
    if (!node) return;

    switch (node->type) {
        case NODE_BINARY_OP: {
            char* temp = new_temp();
            generate_ir(node->left, output);
            generate_ir(node->right, output);
            fprintf(output, "%s = %s %c %s\n", 
                    temp, 
                    node->left->temp_var,
                    node->op,
                    node->right->temp_var);
            node->temp_var = temp;
            break;
        }

        case NODE_IF: {
            char* label_else = new_label();
            char* label_end = new_label();
            
            generate_ir(node->condition, output);
            fprintf(output, "if_false %s goto %s\n", 
                    node->condition->temp_var, 
                    label_else);
            
            generate_ir(node->then_stmt, output);
            fprintf(output, "goto %s\n", label_end);
            
            fprintf(output, "%s:\n", label_else);
            if (node->else_stmt) {
                generate_ir(node->else_stmt, output);
            }
            fprintf(output, "%s:\n", label_end);
            break;
        }

        case NODE_WHILE: {
            char* label_start = new_label();
            char* label_end = new_label();
            
            fprintf(output, "%s:\n", label_start);
            generate_ir(node->condition, output);
            fprintf(output, "if_false %s goto %s\n", 
                    node->condition->temp_var, 
                    label_end);
            
            generate_ir(node->body, output);
            fprintf(output, "goto %s\n", label_start);
            fprintf(output, "%s:\n", label_end);
            break;
        }

        // Add other cases for different node types
    }
}