#include "../parser/ast.h"
#include "semantic_analyzer.h"
#include <stdio.h>
#include<string.h>
void analyze_semantics(ASTNode* root) {
    if (!root) return;

    // Example: very simple semantic check
    switch (root->type) {
        case AST_NUMBER:
        case AST_IDENTIFIER:
            // Leaf nodes, nothing to check here
            break;

        case AST_BINARY_OP:
            analyze_semantics(root->binop.left);
            analyze_semantics(root->binop.right);
            break;

        case AST_DECLARATION:
            // Check variable declaration validity
            if (!root->declaration.name) {
                fprintf(stderr, "Semantic error: declaration without name\n");
            }
            analyze_semantics(root->declaration.init);
            break;

        case AST_ASSIGNMENT:
            if (!root->assignment.name) {
                fprintf(stderr, "Semantic error: assignment without variable name\n");
            }
            analyze_semantics(root->assignment.value);
            break;

        case AST_COMPOUND:
            for (size_t i = 0; i < root->compound.count; ++i) {
                analyze_semantics(root->compound.statements[i]);
            }
            break;

        default:
            fprintf(stderr, "Semantic error: unknown AST node type\n");
    }
}
