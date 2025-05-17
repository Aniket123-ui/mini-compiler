#include <stdlib.h>
#include <string.h>
#include "semantic_analyzer.h"
#include <stdio.h>

void analyze_semantics(ASTNode* root) {
    // Basic semantic check (stub)
    if (!root) {
        printf("Semantic Error: Empty AST\n");
        return;
    }
    // Future checks can be added here
}

SemanticAnalyzer* create_semantic_analyzer(void) {
    SemanticAnalyzer* analyzer = malloc(sizeof(SemanticAnalyzer));
    analyzer->current_scope = create_symbol_table(NULL); // global scope
    analyzer->error_count = 0;
    return analyzer;
}

static void report_error(SemanticAnalyzer* analyzer, const char* message, Token* token) {
    printf("Semantic Error (Line %d, Col %d): %s\n", token->line, token->column, message);
    analyzer->error_count++;
}

static void analyze_expression(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            break;
        case AST_IDENTIFIER: {
            if (!lookup_symbol(analyzer->current_scope, node->name)) {
                report_error(analyzer, "Undeclared variable", node->token);
            }
            break;
        }
        case AST_BINOP:
            analyze_expression(analyzer, node->left);
            analyze_expression(analyzer, node->right);
            break;
        default:
            break;
    }
}

static void analyze_statement(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_DECLARATION: {
            if (lookup_symbol(analyzer->current_scope, node->name)) {
                report_error(analyzer, "Variable already declared", node->token);
            } else {
                insert_symbol(analyzer->current_scope, node->name);
            }
            analyze_expression(analyzer, node->left);
            break;
        }
        case AST_ASSIGNMENT: {
            if (!lookup_symbol(analyzer->current_scope, node->left->name)) {
                report_error(analyzer, "Assignment to undeclared variable", node->token);
            }
            analyze_expression(analyzer, node->right);
            break;
        }
        case AST_IF:
            analyze_expression(analyzer, node->condition);
            analyze_statement(analyzer, node->body);
            analyze_statement(analyzer, node->else_stmt);
            break;
        case AST_WHILE:
            analyze_expression(analyzer, node->condition);
            analyze_statement(analyzer, node->body);
            break;
        case AST_RETURN:
            analyze_expression(analyzer, node->left);
            break;
        case AST_COMPOUND: {
            SymbolTable* prev = analyzer->current_scope;
            analyzer->current_scope = create_symbol_table(prev);
            ASTNode* stmt = node->statements;
            while (stmt) {
                analyze_statement(analyzer, stmt);
                stmt = stmt->right;
            }
            analyzer->current_scope = prev;
            break;
        }
        default:
            break;
    }
}

bool analyze_ast(SemanticAnalyzer* analyzer, ASTNode* root) {
    analyze_statement(analyzer, root);
    return analyzer->error_count == 0;
}

void free_semantic_analyzer(SemanticAnalyzer* analyzer) {
    free_symbol_table(analyzer->current_scope);
    free(analyzer);
}
