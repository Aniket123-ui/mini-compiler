#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

Lexer* create_lexer(const char* input) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->input = strdup(input);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

static void advance(Lexer* lexer) {
    lexer->position++;
    if (lexer->position > strlen(lexer->input) - 1) {
        lexer->position = strlen(lexer->input);
    }
    lexer->column++;
}

static void skip_whitespace(Lexer* lexer) {
    while (lexer->position < strlen(lexer->input) && isspace(lexer->input[lexer->position])) {
        if (lexer->input[lexer->position] == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        advance(lexer);
    }
}

static int get_number(Lexer* lexer) {
    char number[256] = {0};
    int i = 0;
    while (lexer->position < strlen(lexer->input) && 
           isdigit(lexer->input[lexer->position])) {
        number[i++] = lexer->input[lexer->position];
        advance(lexer);
    }
    return atoi(number);
}

Token* get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    
    if (lexer->position >= strlen(lexer->input)) {
        return create_token(TOKEN_EOF, 0, lexer->line, lexer->column);
    }

    char current_char = lexer->input[lexer->position];

    if (isdigit(current_char)) {
        int value = get_number(lexer);
        return create_token(TOKEN_NUMBER, value, lexer->line, lexer->column);
    }

    Token* token = NULL;
    switch (current_char) {
        case '+':
            token = create_token(TOKEN_PLUS, '+', lexer->line, lexer->column);
            break;
        case '-':
            token = create_token(TOKEN_MINUS, '-', lexer->line, lexer->column);
            break;
        case '*':
            token = create_token(TOKEN_MULTIPLY, '*', lexer->line, lexer->column);
            break;
        case '/':
            token = create_token(TOKEN_DIVIDE, '/', lexer->line, lexer->column);
            break;
        default:
            token = create_token(TOKEN_ERROR, current_char, lexer->line, lexer->column);
            break;
    }

    advance(lexer);
    return token;
}

void free_lexer(Lexer* lexer) {
    if (lexer) {
        free((void*)lexer->input);
        free(lexer);
    }
}

Token* create_token(TokenType type, int value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->line = line;
    token->column = column;
    return token;
}

void free_token(Token* token) {
    if (token) {
        free(token);
    }
}