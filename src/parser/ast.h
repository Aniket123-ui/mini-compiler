#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "../lexer/token.h"

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
    OperatorType op_type;
    int value;
    char* name;

    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* condition;
    struct ASTNode* body;
    struct ASTNode* else_stmt;
    struct ASTNode* statements;
    struct ASTNode* parent_expr;

    struct Token* token; // Used for source mapping and error reporting
    char* temp_var;      // Used during IR generation to hold temporary variable names
} ASTNode;

// Constructors
ASTNode* create_number_node(int value, struct Token* token);
ASTNode* create_identifier_node(char* name, struct Token* token);
ASTNode* create_binop_node(OperatorType op, ASTNode* left, ASTNode* right, struct Token* token);
ASTNode* create_assignment_node(ASTNode* identifier, ASTNode* expr, struct Token* token);
ASTNode* create_declaration_node(char* name, ASTNode* expr, struct Token* token);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_stmt, ASTNode* else_stmt);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);
ASTNode* create_return_node(ASTNode* expr);
ASTNode* create_compound_node();
void add_statement_to_block(ASTNode* block, ASTNode* stmt); // Appends stmt to the end of block->statements
void free_ast(ASTNode* node);

#endif // AST_H
