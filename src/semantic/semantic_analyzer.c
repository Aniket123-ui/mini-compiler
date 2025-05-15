#include "semantic_analyzer.h"
#include <stdio.h>
#include <stdlib.h>

// Create semantic analyzer and initialize symbol table
SemanticAnalyzer* create_semantic_analyzer(void) {
    SemanticAnalyzer* analyzer = (SemanticAnalyzer*)malloc(sizeof(SemanticAnalyzer));
    analyzer->current_scope = create_symbol_table(64);
    analyzer->error_count = 0;
    return analyzer;
}

// Recursive AST analyzer - example for number and binop nodes
bool analyze_ast(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node) return true;

    switch (node->type) {
        case AST_NUMBER:
            // Numbers are always valid
            return true;

        case AST_BINOP:
            if (!analyze_ast(analyzer, node->left)) return false;
            if (!analyze_ast(analyzer, node->right)) return false;
            return true;

        // Add more AST node type cases as needed

        default:
            printf("Semantic Analyzer Error: Unknown node type %d\n", node->type);
            analyzer->error_count++;
            return false;
    }
}

void free_semantic_analyzer(SemanticAnalyzer* analyzer) {
    if (!analyzer) return;
    free_symbol_table(analyzer->current_scope);
    free(analyzer);
}
