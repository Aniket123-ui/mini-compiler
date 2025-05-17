#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include "../lexer/token.h"

#ifdef _WIN32
#define strdup _strdup
#endif

ASTNode* create_number_node(int value, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_NUMBER;
    node->number.value = value;       // Corrected access here
    node->token = token;
    return node;
}

ASTNode* create_identifier_node(char* name, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_IDENTIFIER;
    node->identifier.name = strdup(name);  // strdup for name duplication
    node->token = token;
    return node;
}

ASTNode* create_binop_node(OperatorType op, ASTNode* left, ASTNode* right, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_BINOP;
    node->binop.op_type = op;
    node->binop.left = left;
    node->binop.right = right;
    node->token = token;
    return node;
}

ASTNode* create_declaration_node(char* name, ASTNode* init, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_DECLARATION;
    node->declaration.name = strdup(name);  // strdup to duplicate string
    node->declaration.init = init;
    node->token = token;
    return node;
}

ASTNode* create_assignment_node(ASTNode* identifier, ASTNode* value, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_ASSIGNMENT;
    node->assignment.identifier = identifier;
    node->assignment.value = value;
    node->token = token;
    return node;
}

ASTNode* create_compound_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_COMPOUND;
    node->compound.statements = NULL;
    node->compound.count = 0;
    node->token = NULL;
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_IDENTIFIER:
            free(node->identifier.name);
            break;

        case AST_BINOP:
            free_ast(node->binop.left);
            free_ast(node->binop.right);
            break;

        case AST_DECLARATION:
            free(node->declaration.name);
            if (node->declaration.init)
                free_ast(node->declaration.init);
            break;

        case AST_ASSIGNMENT:
            free_ast(node->assignment.identifier);
            free_ast(node->assignment.value);
            break;

        case AST_COMPOUND:
            for (size_t i = 0; i < node->compound.count; ++i)
                free_ast(node->compound.statements[i]);
            free(node->compound.statements);
            break;

        case AST_NUMBER:
            // No dynamic memory to free
            break;

        default:
            break;
    }

    free(node);
}
