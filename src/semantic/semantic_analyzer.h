#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <stdbool.h>
#include "../parser/ast.h"
#include "../utils/symbol_table.h"  // Ensure this path is correct and file exists

typedef struct {
    SymbolTable* current_scope;
    int error_count;
} SemanticAnalyzer;

SemanticAnalyzer* create_semantic_analyzer(void);
bool analyze_ast(SemanticAnalyzer* analyzer, ASTNode* node);
void free_semantic_analyzer(SemanticAnalyzer* analyzer);

#endif  // SEMANTIC_ANALYZER_H
