#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    AST_NUMBER,
    AST_BINOP
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode* left;
    struct ASTNode* right;
    Token* token;
} ASTNode;

typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

Parser* create_parser(Lexer* lexer);
ASTNode* parse(Parser* parser);
void free_parser(Parser* parser);
void free_ast(ASTNode* node);

#endif