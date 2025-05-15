#include "semantic_analyzer.h"
#include "../utils/error_handler.h"
#include <stdlib.h>

SemanticAnalyzer* create_semantic_analyzer(void) {
    SemanticAnalyzer* analyzer = malloc(sizeof(SemanticAnalyzer));
    analyzer->current_scope = create_symbol_table(211); // Prime number size
    analyzer->error_count = 0;
    return analyzer;
}

static bool analyze_declaration(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node->identifier) {
        report_error(ERROR_SEMANTIC, node->line, node->column,
                    "Missing identifier in declaration");
        return false;
    }

    // Check for duplicate declaration
    SymbolEntry* existing = lookup_symbol(analyzer->current_scope, node->identifier);
    if (existing && existing->scope_level == analyzer->current_scope->scope_level) {
        report_error(ERROR_REDEFINITION, node->line, node->column,
                    "Variable '%s' already declared in this scope", node->identifier);
        return false;
    }

    // Add to symbol table
    return insert_symbol(analyzer->current_scope, node->identifier, 
                        SYMBOL_VARIABLE, node->data_type);
}

static DataType check_expression_type(SemanticAnalyzer* analyzer, ASTNode* expr) {
    switch (expr->type) {
        case AST_NUMBER:
            return TYPE_INT;

        case AST_IDENTIFIER: {
            SymbolEntry* symbol = lookup_symbol(analyzer->current_scope, expr->identifier);
            if (!symbol) {
                report_error(ERROR_UNDEFINED_VAR, expr->line, expr->column,
                            "Use of undeclared variable '%s'", expr->identifier);
                return TYPE_ERROR;
            }
            return symbol->data_type;
        }

        case AST_BINARY_OP: {
            DataType left_type = check_expression_type(analyzer, expr->left);
            DataType right_type = check_expression_type(analyzer, expr->right);

            if (left_type == TYPE_ERROR || right_type == TYPE_ERROR)
                return TYPE_ERROR;

            // Type compatibility check for binary operations
            if (left_type != right_type) {
                report_error(ERROR_TYPE, expr->line, expr->column,
                            "Type mismatch in binary operation");
                return TYPE_ERROR;
            }

            // For comparison operators
            if (expr->op_type == OP_GT || expr->op_type == OP_LT || 
                expr->op_type == OP_GE || expr->op_type == OP_LE || 
                expr->op_type == OP_EQ || expr->op_type == OP_NE) {
                return TYPE_BOOL;
            }

            return left_type;
        }

        default:
            return TYPE_ERROR;
    }
}

static bool analyze_assignment(SemanticAnalyzer* analyzer, ASTNode* node) {
    // Check if variable exists
    SymbolEntry* symbol = lookup_symbol(analyzer->current_scope, node->left->identifier);
    if (!symbol) {
        report_error(ERROR_UNDEFINED_VAR, node->line, node->column,
                    "Assignment to undeclared variable '%s'", node->left->identifier);
        return false;
    }

    // Type check
    DataType expr_type = check_expression_type(analyzer, node->right);
    if (expr_type != symbol->data_type) {
        report_error(ERROR_TYPE, node->line, node->column,
                    "Type mismatch in assignment to '%s'", node->left->identifier);
        return false;
    }

    return true;
}

bool analyze_ast(SemanticAnalyzer* analyzer, ASTNode* node) {
    if (!node) return true;

    bool result = true;

    switch (node->type) {
        case AST_DECLARATION:
            result = analyze_declaration(analyzer, node);
            break;

        case AST_ASSIGNMENT:
            result = analyze_assignment(analyzer, node);
            break;

        case AST_IF:
        case AST_WHILE: {
            // Condition must be boolean
            DataType cond_type = check_expression_type(analyzer, node->condition);
            if (cond_type != TYPE_BOOL) {
                report_error(ERROR_TYPE, node->line, node->column,
                            "Condition must be a boolean expression");
                result = false;
            }
            
            // Enter new scope for body
            enter_scope(analyzer->current_scope);
            result &= analyze_ast(analyzer, node->body);
            exit_scope(analyzer->current_scope);
            break;
        }

        case AST_COMPOUND: {
            enter_scope(analyzer->current_scope);
            // Analyze each statement in the compound
            ASTNode* stmt = node->statements;
            while (stmt) {
                result &= analyze_ast(analyzer, stmt);
                stmt = stmt->next;
            }
            exit_scope(analyzer->current_scope);
            break;
        }
    }

    return result;
}

void free_semantic_analyzer(SemanticAnalyzer* analyzer) {
    if (analyzer) {
        free_symbol_table(analyzer->current_scope);
        free(analyzer);
    }
}