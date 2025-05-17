#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct Lexer {
    const char* input;  // The source code string
    int position;       // Current index in input
    int line;
    int column;
} Lexer;

Lexer* create_lexer(const char* input);
Token* get_next_token(Lexer* lexer);
void free_token(Token* token);
void free_lexer(Lexer* lexer);
Lexer* create_lexer_from_file(const char* filename);
Token* lexer_next_token(Lexer* lexer);     // Needed for parser
Token* lexer_peek_token(Lexer* lexer);     // Needed for lookahead

Token* create_token(TokenType type, const char* value, int line, int column);


#endif // LEXER_H
