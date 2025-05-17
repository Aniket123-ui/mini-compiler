#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifndef HAVE_STRNDUP
// Provide strndup if not available (especially on Windows)
char* strndup(const char* s, size_t n) {
    size_t len = strlen(s);
    if (len > n) len = n;
    char* result = (char*)malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}
#endif

Lexer* create_lexer(const char* input) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->input = strdup(input);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

void advance(Lexer* lexer) {
    if (lexer->input[lexer->position] == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->position++;
}

void skip_whitespace(Lexer* lexer) {
    while (isspace(lexer->input[lexer->position])) {
        advance(lexer);
    }
}

Token* create_token(TokenType type, const char* value, int line, int column) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value); // dynamically duplicate string
    token->line = line;
    token->column = column;
    return token;
}

Token* get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    char current = lexer->input[lexer->position];
    int line = lexer->line;
    int col = lexer->column;

    if (isdigit(current)) {
        int start = lexer->position;
        while (isdigit(lexer->input[lexer->position])) {
            advance(lexer);
        }
        int length = lexer->position - start;
        char* value = strndup(&lexer->input[start], length);
        return create_token(TOKEN_NUMBER, value, line, col);
    }

    if (isalpha(current)) {
        int start = lexer->position;
        while (isalnum(lexer->input[lexer->position])) {
            advance(lexer);
        }
        int length = lexer->position - start;
        char* value = strndup(&lexer->input[start], length);
        if (strcmp(value, "int") == 0) {
            return create_token(TOKEN_INT, value, line, col);  // TOKEN_INT = 16
        }
        return create_token(TOKEN_IDENTIFIER, value, line, col);
    }

    switch (current) {
        case '+': advance(lexer); return create_token(TOKEN_PLUS, "+", line, col);
        case '-': advance(lexer); return create_token(TOKEN_MINUS, "-", line, col);
        case '*': advance(lexer); return create_token(TOKEN_MUL, "*", line, col);
        case '/': advance(lexer); return create_token(TOKEN_DIV, "/", line, col);
        case '=': advance(lexer); return create_token(TOKEN_ASSIGN, "=", line, col);
        case ';': advance(lexer); return create_token(TOKEN_SEMICOLON, ";", line, col);
        case '(': advance(lexer); return create_token(TOKEN_LPAREN, "(", line, col);
        case ')': advance(lexer); return create_token(TOKEN_RPAREN, ")", line, col);
        case '{': advance(lexer); return create_token(TOKEN_LBRACE, "{", line, col); // <-- NEW
        case '}': advance(lexer); return create_token(TOKEN_RBRACE, "}", line, col); // <-- NEW
        case '\0': return create_token(TOKEN_EOF, "", line, col);
        default:
            advance(lexer);
            return create_token(TOKEN_ERROR, "?", line, col);
    }
    
}

void free_token(Token* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}
