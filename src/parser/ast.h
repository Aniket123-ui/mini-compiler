#ifndef AST_H
#define AST_H

#include <stddef.h>  // for size_t
#include "../lexer/token.h"
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_COMPOUND
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} BinOpType;

struct Token;  // Forward declaration

typedef struct ASTNode {
    ASTNodeType type;
    struct Token* token;

    union {
        // For AST_NUMBER
        int value;

        // For AST_IDENTIFIER
        char* identifier;

        // For AST_BINARY_OP
        struct {
            BinOpType op_type;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;

        // For AST_DECLARATION
        struct {
            char* name;
            struct ASTNode* init;
        } declaration;

        // For AST_ASSIGNMENT
        struct {
            char* name;
            struct ASTNode* value;
        } assignment;

        // For AST_COMPOUND
        struct {
            struct ASTNode** statements;
            size_t count;
        } compound;
    };
} ASTNode;

// Function declarations
ASTNode* create_number_node(int value, struct Token* token);
ASTNode* create_identifier_node(const char* name, struct Token* token);
ASTNode* create_binop_node(BinOpType op, ASTNode* left, ASTNode* right, struct Token* token);
ASTNode* create_declaration_node(const char* name, ASTNode* init, struct Token* token);
ASTNode* create_assignment_node(const char* name, ASTNode* value, struct Token* token);
ASTNode* create_compound_node(ASTNode** statements, size_t count);
void free_ast(ASTNode* node);

#endif // AST_H
