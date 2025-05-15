#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_number_node(int value, Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->value = value;
    node->token = token;
    node->left = node->right = NULL;
    node->condition = node->body = node->else_stmt = node->statements = node->parent_expr = NULL;
    node->name = NULL;
    return node;
}

ASTNode* create_identifier_node(char* name, Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->name = strdup(name); // duplicate string for safety
    node->token = token;
    node->left = node->right = NULL;
    node->condition = node->body = node->else_stmt = node->statements = node->parent_expr = NULL;
    node->value = 0;
    return node;
}

ASTNode* create_binop_node(OperatorType op, ASTNode* left, ASTNode* right, Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_BINOP;
    node->op_type = op;
    node->left = left;
    node->right = right;
    node->token = token;
    node->condition = node->body = node->else_stmt = node->statements = node->parent_expr = NULL;
    node->name = NULL;
    node->value = 0;
    return node;
}

ASTNode* create_assignment_node(ASTNode* identifier, ASTNode* expr, Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->left = identifier;
    node->right = expr;
    node->token = token;
    node->condition = node->body = node->else_stmt = node->statements = node->parent_expr = NULL;
    node->name = NULL;
    node->value = 0;
    return node;
}
