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
    AST_COMPOUND, // legacy, will alias to AST_BLOCK
    AST_BLOCK,
    AST_IF,
    AST_WHILE,
    AST_RETURN,
    AST_FUNCTION
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LT,   // <
    OP_GT,   // >
    OP_LE,   // <=
    OP_GE,   // >=
    OP_EQ,   // ==
    OP_NEQ   // !=
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

        // For AST_BLOCK (and legacy AST_COMPOUND)
        struct {
            struct ASTNode** statements;
            size_t count;
        } block;

        // For AST_IF
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;

        // For AST_WHILE
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;

        // For AST_RETURN
        struct {
            struct ASTNode* expr;
        } return_stmt;

        // For AST_FUNCTION
        struct {
            char* name;
            struct ASTNode* body;
        } function;
    };
} ASTNode;

// Function declarations
ASTNode* create_number_node(int value, struct Token* token);
ASTNode* create_identifier_node(const char* name, struct Token* token);
ASTNode* create_binop_node(BinOpType op, ASTNode* left, ASTNode* right, struct Token* token);
ASTNode* create_declaration_node(const char* name, ASTNode* init, struct Token* token);
ASTNode* create_assignment_node(const char* name, ASTNode* value, struct Token* token);
ASTNode* create_compound_node(ASTNode** statements, size_t count);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch, struct Token* token);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body, struct Token* token);
ASTNode* create_return_node(struct ASTNode* expr, struct Token* token);
ASTNode* create_function_node(const char* name, struct ASTNode* body, struct Token* token);
void free_ast(ASTNode* node);

#endif // AST_H
