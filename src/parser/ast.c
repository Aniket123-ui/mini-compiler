#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_number_node(int value, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->value = value;
    node->token = token;
    node->left = node->right = NULL;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_identifier_node(char* name, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->name = strdup(name);
    node->token = token;
    node->left = node->right = NULL;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_binop_node(OperatorType op, ASTNode* left, ASTNode* right, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINOP;
    node->op_type = op;
    node->left = left;
    node->right = right;
    node->token = token;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_assignment_node(ASTNode* identifier, ASTNode* expr, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->left = identifier;
    node->right = expr;
    node->token = token;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_declaration_node(char* name, ASTNode* expr, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_DECLARATION;
    node->name = strdup(name);
    node->right = expr;
    node->token = token;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_if_node(ASTNode* cond, ASTNode* then_stmt, ASTNode* else_stmt) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->condition = cond;
    node->body = then_stmt;
    node->else_stmt = else_stmt;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_while_node(ASTNode* cond, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->condition = cond;
    node->body = body;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_return_node(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->left = expr;
    node->temp_var = NULL;
    return node;
}

ASTNode* create_compound_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_COMPOUND;
    node->statements = NULL;
    node->temp_var = NULL;
    return node;
}

void add_statement_to_block(ASTNode* block, ASTNode* stmt) {
    if (!block->statements) {
        block->statements = stmt;
    } else {
        ASTNode* current = block->statements;
        while (current->right) current = current->right;
        current->right = stmt;
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->condition);
    free_ast(node->body);
    free_ast(node->else_stmt);
    free_ast(node->statements);
    if (node->token) free_token(node->token);
    if (node->name) free(node->name);
    if (node->temp_var) free(node->temp_var);
    free(node);
}
