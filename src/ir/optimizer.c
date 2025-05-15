#include "optimizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  // Add this for bool type

// Forward declarations
static bool is_constant_expression(ASTNode* node);  // Changed return type to bool
static bool has_side_effects(ASTNode* node);
static int evaluate_constant_expression(ASTNode* node);

static int evaluate_constant_expression(ASTNode* node) {
    if (!node) return 0;

    switch (node->type) {
        case AST_NUMBER:
            return node->token->value;

        case AST_BINARY_OP: {
            int left = evaluate_constant_expression(node->left);
            int right = evaluate_constant_expression(node->right);

            switch (node->op_type) {
                case OP_PLUS:  return left + right;
                case OP_MINUS: return left - right;
                case OP_MULTIPLY: return left * right;
                case OP_DIVIDE: 
                    if (right == 0) {
                        fprintf(stderr, "Warning: Division by zero in constant folding\n");
                        return 0;
                    }
                    return left / right;
                default: return 0;
            }
        }

        default:
            return 0;
    }
}

static bool is_constant_expression(ASTNode* node) {  // Changed return type to bool
    if (!node) return true;

    switch (node->type) {
        case AST_NUMBER:
            return true;

        case AST_BINARY_OP:
            return is_constant_expression(node->left) && 
                   is_constant_expression(node->right);

        default:
            return false;
    }
}

ASTNode* constant_folding(ASTNode* node, OptimizationStats* stats) {
    if (!node) return NULL;

    // First, recursively optimize children
    if (node->left) node->left = constant_folding(node->left, stats);
    if (node->right) node->right = constant_folding(node->right, stats);
    if (node->condition) node->condition = constant_folding(node->condition, stats);
    if (node->body) node->body = constant_folding(node->body, stats);

    // Check if this is a constant expression that can be folded
    if (node->type == AST_BINARY_OP && is_constant_expression(node)) {
        int value = evaluate_constant_expression(node);
        
        // Create new token and node
        Token* new_token = create_token(TOKEN_NUMBER, value, node->line, node->column);
        ASTNode* folded = create_number_node(new_token);
        folded->parent_expr = node->parent_expr;  // Preserve parent relationship
        
        stats->constants_folded++;
        stats->modified = true;
        
        // Free the old expression tree
        if (node->token) {
            free_token(node->token);
        }
        node->left = node->right = NULL;  // Prevent recursive free
        free_ast(node);
        
        return folded;
    }

    return node;
}

static bool has_side_effects(ASTNode* node) {
    if (!node) return false;

    switch (node->type) {
        case AST_ASSIGNMENT:
        case AST_FUNCTION:
            return true;

        case AST_BINARY_OP:
            return has_side_effects(node->left) || has_side_effects(node->right);

        default:
            return false;
    }
}

ASTNode* dead_code_elimination(ASTNode* node, OptimizationStats* stats) {
    if (!node) return NULL;

    // Recursively optimize children
    if (node->left) node->left = dead_code_elimination(node->left, stats);
    if (node->right) node->right = dead_code_elimination(node->right, stats);
    if (node->condition) node->condition = dead_code_elimination(node->condition, stats);
    if (node->body) node->body = dead_code_elimination(node->body, stats);

    // Dead code elimination cases
    switch (node->type) {
        case AST_IF: {
            // If condition is constant
            if (is_constant_expression(node->condition)) {
                int result = evaluate_constant_expression(node->condition);
                ASTNode* retained = result ? node->body : node->else_stmt;
                
                if (!retained || !has_side_effects(retained)) {
                    stats->dead_code_removed++;
                    stats->modified = true;
                    free_ast(node);
                    return NULL;
                }
                
                // Keep the branch that will be executed
                ASTNode* temp = retained;
                node->condition = node->body = node->else_stmt = NULL;
                free_ast(node);
                return temp;
            }
            break;
        }

        case AST_WHILE: {
            // If condition is constant false
            if (is_constant_expression(node->condition)) {
                int result = evaluate_constant_expression(node->condition);
                if (!result) {
                    stats->dead_code_removed++;
                    stats->modified = true;
                    free_ast(node);
                    return NULL;
                }
            }
            break;
        }

        case AST_BINARY_OP: {
            // Remove operations with no side effects and unused results
            if (!has_side_effects(node) && node->parent_expr == NULL) {
                stats->dead_code_removed++;
                stats->modified = true;
                free_ast(node);
                return NULL;
            }
            break;
        }
    }

    return node;
}

ASTNode* optimize_ir(ASTNode* ast, OptimizationStats* stats) {
    if (!ast) return NULL;

    stats->modified = false;
    stats->constants_folded = 0;
    stats->dead_code_removed = 0;

    // Apply optimizations repeatedly until no more changes
    do {
        stats->modified = false;
        ast = constant_folding(ast, stats);
        ast = dead_code_elimination(ast, stats);
    } while (stats->modified);

    return ast;
}