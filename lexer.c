#include "lexer.h"

Lexer* create_lexer(char* text) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->text = strdup(text);
    lexer->pos = 0;
    lexer->current_char = text[0];
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

static void advance(Lexer* lexer) {
    lexer->pos++;
    if (lexer->pos > strlen(lexer->text) - 1) {
        lexer->current_char = '\0';
    } else {
        lexer->current_char = lexer->text[lexer->pos];
        lexer->column++;
    }
}

static void skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        advance(lexer);
    }
}

static int get_number(Lexer* lexer) {
    char number[256] = {0};
    int i = 0;
    while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
        number[i++] = lexer->current_char;
        advance(lexer);
    }
    return atoi(number);
}

Token* get_next_token(Lexer* lexer) {
    Token* token = (Token*)malloc(sizeof(Token));
    
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        if (isdigit(lexer->current_char)) {
            token->type = TOKEN_NUMBER;
            token->value = get_number(lexer);
            token->line = lexer->line;
            token->column = lexer->column;
            return token;
        }

        if (lexer->current_char == '+') {
            token->type = TOKEN_PLUS;
            token->value = '+';
            token->line = lexer->line;
            token->column = lexer->column;
            advance(lexer);
            return token;
        }

        if (lexer->current_char == '-') {
            token->type = TOKEN_MINUS;
            token->value = '-';
            token->line = lexer->line;
            token->column = lexer->column;
            advance(lexer);
            return token;
        }

        printf("Error: Invalid character at line %d, column %d\n", lexer->line, lexer->column);
        exit(1);
    }

    token->type = TOKEN_EOF;
    token->value = 0;
    token->line = lexer->line;
    token->column = lexer->column;
    return token;
}

void free_lexer(Lexer* lexer) {
    free(lexer->text);
    free(lexer);
}