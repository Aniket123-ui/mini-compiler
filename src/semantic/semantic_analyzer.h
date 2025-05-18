#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "..\utils\symbol_table.h"
#include "..\parser\ast.h"


void analyze_semantics(ASTNode* root);
int get_semantic_error();


typedef struct {
    SymbolTable* table;   // This field MUST be present
} SemanticAnalyzer;

SemanticAnalyzer* create_semantic_analyzer();
void analyze(SemanticAnalyzer* analyzer, ASTNode* root);
void analyze_statement(SemanticAnalyzer* analyzer, ASTNode* node);
void analyze_expression(SemanticAnalyzer* analyzer, ASTNode* node);
void free_semantic_analyzer(SemanticAnalyzer* analyzer);

#endif
