#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"

typedef struct Parser {
    Lexer* lexer;
    Token* current_token;
} Parser;

// Now create_parser takes Lexer* pointer as argument
Parser* create_parser(Lexer* lexer);

ASTNode* parse(Parser* parser);

void free_parser(Parser* parser);

#endif // PARSER_H
