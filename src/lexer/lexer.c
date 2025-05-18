#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifdef _WIN32
#define strdup _strdup
#endif

#ifndef HAVE_STRNDUP
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
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    return token;
}

TokenType check_keyword(const char* str) {
    if (strcmp(str, "if") == 0) return TOKEN_IF;
    if (strcmp(str, "else") == 0) return TOKEN_ELSE;
    if (strcmp(str, "while") == 0) return TOKEN_WHILE;
    if (strcmp(str, "return") == 0) return TOKEN_RETURN;
    if (strcmp(str, "int") == 0) return TOKEN_INT;
    return TOKEN_IDENTIFIER;
}

Token* get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    char current = lexer->input[lexer->position];
    int line = lexer->line;
    int col = lexer->column;

    if (current == '\0') {
        return create_token(TOKEN_EOF, "", line, col);
    }

    if (isdigit(current)) {
        int start = lexer->position;
        while (isdigit(lexer->input[lexer->position])) {
            advance(lexer);
        }
        int length = lexer->position - start;
        char* value = strndup(&lexer->input[start], length);
        return create_token(TOKEN_NUMBER, value, line, col);
    }

    if (isalpha(current) || current == '_') {
        int start = lexer->position;
        while (isalnum(lexer->input[lexer->position]) || lexer->input[lexer->position] == '_') {
            advance(lexer);
        }
        int length = lexer->position - start;
        char* value = strndup(&lexer->input[start], length);
        TokenType type = check_keyword(value);
        return create_token(type, value, line, col);
    }

    if (current == '<') {
        advance(lexer);
        if (lexer->input[lexer->position] == '=') {
            advance(lexer);
            return create_token(TOKEN_LE, "<=", line, col);
        }
        return create_token(TOKEN_LT, "<", line, col);
    }

    if (current == '>') {
        advance(lexer);
        if (lexer->input[lexer->position] == '=') {
            advance(lexer);
            return create_token(TOKEN_GE, ">=", line, col);
        }
        return create_token(TOKEN_GT, ">", line, col);
    }

    if (current == '=') {
        advance(lexer);
        if (lexer->input[lexer->position] == '=') {
            advance(lexer);
            return create_token(TOKEN_EQ, "==", line, col);
        }
        return create_token(TOKEN_ASSIGN, "=", line, col);
    }

    if (current == '!') {
        advance(lexer);
        if (lexer->input[lexer->position] == '=') {
            advance(lexer);
            return create_token(TOKEN_NEQ, "!=", line, col);
        }
        return create_token(TOKEN_ERROR, "!", line, col);
    }

    switch (current) {
        case '+': advance(lexer); return create_token(TOKEN_PLUS, "+", line, col);
        case '-': advance(lexer); return create_token(TOKEN_MINUS, "-", line, col);
        case '*': advance(lexer); return create_token(TOKEN_MUL, "*", line, col);
        case '/': advance(lexer); return create_token(TOKEN_DIV, "/", line, col);
        case ';': advance(lexer); return create_token(TOKEN_SEMICOLON, ";", line, col);
        case '(': advance(lexer); return create_token(TOKEN_LPAREN, "(", line, col);
        case ')': advance(lexer); return create_token(TOKEN_RPAREN, ")", line, col);
        case '{': advance(lexer); return create_token(TOKEN_LBRACE, "{", line, col);
        case '}': advance(lexer); return create_token(TOKEN_RBRACE, "}", line, col);
        case ',': advance(lexer); return create_token(TOKEN_PUNCTUATION, ",", line, col);
        default: {
            char unknown[2] = { current, '\0' };
            advance(lexer);
            return create_token(TOKEN_ERROR, unknown, line, col);
        }
    }
}

void free_token(Token* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

void free_lexer(Lexer* lexer) {
    if (lexer) {
        free((void*)lexer->input);
        free(lexer);
    }
}

Lexer* create_lexer_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("File open failed");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    // Skip UTF-8 BOM if present
    if ((unsigned char)buffer[0] == 0xEF &&
        (unsigned char)buffer[1] == 0xBB &&
        (unsigned char)buffer[2] == 0xBF) {
        Lexer* lexer = create_lexer(buffer + 3);
        free(buffer);
        return lexer;
    }

    return create_lexer(buffer);
}

Token* lexer_next_token(Lexer* lexer) {
    return get_next_token(lexer);
}

Token* lexer_peek_token(Lexer* lexer) {
    int old_pos = lexer->position;
    int old_line = lexer->line;
    int old_col = lexer->column;

    Token* token = get_next_token(lexer);

    lexer->position = old_pos;
    lexer->line = old_line;
    lexer->column = old_col;

    return token;
}
