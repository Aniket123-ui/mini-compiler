#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "../lexer/token.h"
 // You must have a token struct for source tracking

// Enumerated types for different AST nodes
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINOP,
    AST_ASSIGNMENT,
    AST_DECLARATION,
    AST_IF,
    AST_WHILE,
    AST_FUNCTION,
    AST_COMPOUND,
    AST_BINARY_OP,
    AST_RETURN
    // Add more as needed
} ASTNodeType;

// Enumerated type for binary operations
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_GT,
    OP_LT,
    OP_GE,
    OP_LE,
    OP_EQ,
    OP_NE
} OperatorType;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    OperatorType op_type;   // Used if type == AST_BINOP or AST_BINARY_OP

    int value;              // For AST_NUMBER
    char* name;             // For AST_IDENTIFIER

    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* condition;
    struct ASTNode* body;
    struct ASTNode* else_stmt;
    struct ASTNode* statements;  // For compound blocks
    struct ASTNode* parent_expr; // Optional, used in optimizer

    struct Token* token;  
    char* temp_var;  // Optional: useful for error reporting
} ASTNode;

// Constructors
ASTNode* create_number_node(int value, struct Token* token);
ASTNode* create_identifier_node(char* name, struct Token* token);
ASTNode* create_binop_node(OperatorType op, ASTNode* left, ASTNode* right, struct Token* token);
ASTNode* create_assignment_node(ASTNode* identifier, ASTNode* expr, struct Token* token);
// Add more constructors as needed

#endif // AST_
