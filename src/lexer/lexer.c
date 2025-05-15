#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer* create_lexer(const char* input) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->input = input;  // No strdup needed here if input lives longer than lexer
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

void free_lexer(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

static void advance(Lexer* lexer) {
    if (lexer->input[lexer->position] == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->position++;
}

static void skip_whitespace(Lexer* lexer) {
    while (lexer->input[lexer->position] && isspace(lexer->input[lexer->position])) {
        advance(lexer);
    }
}

static Token* get_number(Lexer* lexer) {
    int start = lexer->position;
    while (isdigit(lexer->input[lexer->position])) {
        advance(lexer);
    }
    int length = lexer->position - start;

    char* num_str = (char*)malloc(length + 1);
    strncpy(num_str, lexer->input + start, length);
    num_str[length] = '\0';

    Token* token = create_token(TOKEN_NUMBER, num_str, lexer->line, lexer->column - length);
    free(num_str);
    return token;
}

Token* get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);

    if (lexer->input[lexer->position] == '\0') {
        return create_token(TOKEN_EOF, NULL, lexer->line, lexer->column);
    }

    char current_char = lexer->input[lexer->position];

    if (isdigit(current_char)) {
        return get_number(lexer);
    }

    char buf[2] = { current_char, '\0' };

    Token* token = NULL;

    switch (current_char) {
        case '+':
            token = create_token(TOKEN_PLUS, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case '-':
            token = create_token(TOKEN_MINUS, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case '*':
            token = create_token(TOKEN_MUL, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case '/':
            token = create_token(TOKEN_DIV, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case '=':
            token = create_token(TOKEN_ASSIGN, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case ';':
            token = create_token(TOKEN_SEMICOLON, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case '(':
            token = create_token(TOKEN_LPAREN, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        case ')':
            token = create_token(TOKEN_RPAREN, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
        default:
            token = create_token(TOKEN_ERROR, buf, lexer->line, lexer->column);
            advance(lexer);
            break;
    }

    return token;
}

Token* create_token(TokenType type, const char* value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    if (value) {
        token->value = strdup(value);
    } else {
        token->value = NULL;
    }
    token->line = line;
    token->column = column;
    return token;
}

void free_token(Token* token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}
