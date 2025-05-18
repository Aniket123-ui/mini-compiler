#include "../parser/ast.h"
#include "semantic_analyzer.h"
#include "../utils/symbol_table.h"
#include <stdio.h>
#include <string.h>

static int semantic_error = 0;

static void analyze_semantics_node(ASTNode* root, SymbolTable* table) {
    if (!root) return;
    switch (root->type) {
        case AST_NUMBER:
            // Only int supported, always valid
            break;
        case AST_IDENTIFIER:
            if (!lookup_symbol(table, root->identifier)) {
                fprintf(stderr, "Semantic error: variable '%s' used before declaration\n", root->identifier);
                semantic_error = 1;
            }
            break;
        case AST_BINARY_OP:
            analyze_semantics_node(root->binop.left, table);
            analyze_semantics_node(root->binop.right, table);
            // Only int supported, so no further type checks
            break;
        case AST_COMPOUND: {
            SymbolTable* local = create_symbol_table(table);
            for (size_t i = 0; i < root->compound.count; ++i) {
                analyze_semantics_node(root->compound.statements[i], local);
            }
            free_symbol_table(local);
            break;
        }
        case AST_DECLARATION:
            if (!root->declaration.name) {
                fprintf(stderr, "Semantic error: declaration without name\n");
                semantic_error = 1;
            } else {
                if (!insert_symbol(table, root->declaration.name)) {
                    fprintf(stderr, "Semantic error: redeclaration of variable '%s'\n", root->declaration.name);
                    semantic_error = 1;
                }
            }
            analyze_semantics_node(root->declaration.init, table);
            break;
        case AST_ASSIGNMENT:
            if (!root->assignment.name) {
                fprintf(stderr, "Semantic error: assignment without variable name\n");
                semantic_error = 1;
            } else if (!lookup_symbol(table, root->assignment.name)) {
                fprintf(stderr, "Semantic error: assignment to undeclared variable '%s'\n", root->assignment.name);
                semantic_error = 1;
            }
            analyze_semantics_node(root->assignment.value, table);
            break;
        case AST_IF:
            analyze_semantics_node(root->if_stmt.condition, table);
            analyze_semantics_node(root->if_stmt.then_branch, table);
            if (root->if_stmt.else_branch)
                analyze_semantics_node(root->if_stmt.else_branch, table);
            break;
        case AST_WHILE:
            analyze_semantics_node(root->while_stmt.condition, table);
            analyze_semantics_node(root->while_stmt.body, table);
            break;
        default:
            fprintf(stderr, "Semantic error: unknown AST node type %d\n", (int)root->type);
            semantic_error = 1;
            break;
    }
}

void analyze_semantics(ASTNode* root) {
    fprintf(stderr, "[DEBUG] Entering analyze_semantics\n");
    SymbolTable* global = create_symbol_table(NULL);
    analyze_semantics_node(root, global);
    free_symbol_table(global);
    fprintf(stderr, "[DEBUG] Exiting analyze_semantics\n");
}

int get_semantic_error() {
    return semantic_error;
}
