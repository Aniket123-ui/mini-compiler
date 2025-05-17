#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"

typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

Parser* create_parser(Lexer* lexer);
ASTNode* parse(Parser* parser); // Parses an entire program/block
void free_parser(Parser* parser);

#endif
