#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    int value;
    int line;
    int column;
} Token;

typedef struct {
    char* text;
    int pos;
    char current_char;
    int line;
    int column;
} Lexer;

Lexer* create_lexer(char* text);
Token* get_next_token(Lexer* lexer);
void free_lexer(Lexer* lexer);

#endif