#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../lexer/token.h"

#ifdef _WIN32
#define strdup _strdup
#endif

ASTNode* create_number_node(int value, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_NUMBER;
    node->value = value;
    node->token = token;
    return node;
}

ASTNode* create_identifier_node(const char* name, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_IDENTIFIER;
    node->identifier = strdup(name);
    node->token = token;
    return node;
}

ASTNode* create_binop_node(BinOpType op, ASTNode* left, ASTNode* right, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_BINARY_OP;
    node->binop.op_type = op;
    node->binop.left = left;
    node->binop.right = right;
    node->token = token;
    return node;
}

ASTNode* create_declaration_node(const char* name, ASTNode* init, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_DECLARATION;
    node->declaration.name = strdup(name);
    node->declaration.init = init;
    node->token = token;
    return node;
}

ASTNode* create_assignment_node(const char* name, ASTNode* value, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_ASSIGNMENT;
    node->assignment.name = strdup(name);
    node->assignment.value = value;
    node->token = token;
    return node;
}

ASTNode* create_compound_node(ASTNode** statements, size_t count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_BLOCK;
    node->block.statements = statements;
    node->block.count = count;
    node->token = NULL;
    return node;
}

ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_IF;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    node->token = token;
    return node;
}

ASTNode* create_while_node(ASTNode* condition, ASTNode* body, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_WHILE;
    node->while_stmt.condition = condition;
    node->while_stmt.body = body;
    node->token = token;
    return node;
}

ASTNode* create_function_node(const char* name, ASTNode* body, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_FUNCTION;
    node->function.name = strdup(name);
    node->function.body = body;
    node->token = token;
    return node;
}

ASTNode* create_return_node(ASTNode* expr, Token* token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = AST_RETURN;
    node->return_stmt.expr = expr;
    node->token = token;
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_BLOCK:
            for (size_t i = 0; i < node->block.count; ++i)
                free_ast(node->block.statements[i]);
            free(node->block.statements);
            break;
        case AST_COMPOUND:
            for (size_t i = 0; i < node->block.count; ++i)
                free_ast(node->block.statements[i]);
            free(node->block.statements);
            break;
        case AST_IDENTIFIER:
            free(node->identifier);
            break;
        case AST_BINARY_OP:
            free_ast(node->binop.left);
            free_ast(node->binop.right);
            break;
        case AST_DECLARATION:
            free(node->declaration.name);
            if (node->declaration.init)
                free_ast(node->declaration.init);
            break;
        case AST_ASSIGNMENT:
            free(node->assignment.name);
            free_ast(node->assignment.value);
            break;
        case AST_NUMBER:
            // No dynamic memory to free
            break;
        case AST_FUNCTION:
            free(node->function.name);
            free_ast(node->function.body);
            break;
        case AST_RETURN:
            free_ast(node->return_stmt.expr);
            break;
        default:
            break;
    }
    free(node);
}

void print_ast(ASTNode* node, int indent) {
    if (!node) {
        fprintf(stderr, "%*sNULL\n", indent, "");
        return;
    }
    fprintf(stderr, "%*sNode type: %d\n", indent, "", node->type);
    switch (node->type) {
        case AST_NUMBER:
            fprintf(stderr, "%*sNUMBER: %d\n", indent+2, "", node->value);
            break;
        case AST_IDENTIFIER:
            fprintf(stderr, "%*sIDENTIFIER: %s\n", indent+2, "", node->identifier);
            break;
        case AST_BINARY_OP:
            fprintf(stderr, "%*sBINARY_OP\n", indent+2, "");
            print_ast(node->binop.left, indent+4);
            print_ast(node->binop.right, indent+4);
            break;
        case AST_ASSIGNMENT:
            fprintf(stderr, "%*sASSIGNMENT: %s\n", indent+2, "", node->assignment.name);
            print_ast(node->assignment.value, indent+4);
            break;
        case AST_DECLARATION:
            fprintf(stderr, "%*sDECLARATION: %s\n", indent+2, "", node->declaration.name);
            print_ast(node->declaration.init, indent+4);
            break;
        case AST_COMPOUND:
            fprintf(stderr, "%*sCOMPOUND (%zu statements)\n", indent+2, "", node->block.count);
            for (size_t i = 0; i < node->block.count; ++i) {
                print_ast(node->block.statements[i], indent+4);
            }
            break;
        case AST_IF:
            fprintf(stderr, "%*sIF_STATEMENT\n", indent+2, "");
            fprintf(stderr, "%*sCONDITION:\n", indent+4, "");
            print_ast(node->if_stmt.condition, indent+6);
            fprintf(stderr, "%*sTHEN_BRANCH:\n", indent+4, "");
            print_ast(node->if_stmt.then_branch, indent+6);
            if (node->if_stmt.else_branch) {
                fprintf(stderr, "%*sELSE_BRANCH:\n", indent+4, "");
                print_ast(node->if_stmt.else_branch, indent+6);
            }
            break;
        case AST_WHILE:
            fprintf(stderr, "%*sWHILE_STATEMENT\n", indent+2, "");
            fprintf(stderr, "%*sCONDITION:\n", indent+4, "");
            print_ast(node->while_stmt.condition, indent+6);
            fprintf(stderr, "%*sBODY:\n", indent+4, "");
            print_ast(node->while_stmt.body, indent+6);
            break;
        case AST_FUNCTION:
            fprintf(stderr, "%*sFUNCTION: %s\n", indent+2, "", node->function.name);
            fprintf(stderr, "%*sBODY:\n", indent+4, "");
            print_ast(node->function.body, indent+6);
            break;
        case AST_RETURN:
            fprintf(stderr, "%*sRETURN_STATEMENT\n", indent+2, "");
            if (node->return_stmt.expr) {
                fprintf(stderr, "%*sEXPRESSION:\n", indent+4, "");
                print_ast(node->return_stmt.expr, indent+6);
            }
            break;
        default:
            fprintf(stderr, "%*sUNKNOWN TYPE: %d\n", indent+2, "", node->type);
            break;
    }
}
