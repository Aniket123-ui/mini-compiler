#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Forward declarations
static void eat(Parser* parser, TokenType token_type);
static ASTNode* factor(Parser* parser);

Parser* create_parser(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = get_next_token(lexer);
    return parser;
}

static void eat(Parser* parser, TokenType token_type) {
    if (parser->current_token->type == token_type) {
        free_token(parser->current_token);
        parser->current_token = get_next_token(parser->lexer);
    } else {
        printf("Parser Error: Unexpected token at line %d column %d\n", 
            parser->current_token->line, parser->current_token->column);
        exit(EXIT_FAILURE);
    }
}

static ASTNode* factor(Parser* parser) {
    Token* token = parser->current_token;
    if (token->type == TOKEN_NUMBER) {
        ASTNode* node = create_number_node(atoi(token->value), token);
        eat(parser, TOKEN_NUMBER);
        return node;
    }
    printf("Parser Error: Invalid factor at line %d column %d\n", token->line, token->column);
    exit(EXIT_FAILURE);
}

ASTNode* parse(Parser* parser) {
    ASTNode* node = factor(parser);

    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS) {
        Token* token = parser->current_token;

        OperatorType op = (token->type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
        eat(parser, token->type);

        ASTNode* right = factor(parser);
        ASTNode* new_node = create_binop_node(op, node, right, token);
        node = new_node;
    }

    return node;
}

void free_parser(Parser* parser) {
    if (!parser) return;
    if (parser->current_token) {
        free_token(parser->current_token);
    }
    free(parser);
}
