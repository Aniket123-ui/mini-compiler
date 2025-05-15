#include "parser.h"

Parser* create_parser(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = get_next_token(lexer);
    return parser;
}

static void eat(Parser* parser, TokenType token_type) {
    if (parser->current_token->type == token_type) {
        free(parser->current_token);
        parser->current_token = get_next_token(parser->lexer);
    } else {
        printf("Error: Unexpected token\n");
        exit(1);
    }
}

static ASTNode* factor(Parser* parser) {
    Token* token = parser->current_token;
    if (token->type == TOKEN_NUMBER) {
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        eat(parser, TOKEN_NUMBER);
        return node;
    }
    printf("Error: Invalid factor\n");
    exit(1);
}

ASTNode* parse(Parser* parser) {
    ASTNode* node = factor(parser);

    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS) {
        Token* token = parser->current_token;
        if (token->type == TOKEN_PLUS) {
            eat(parser, TOKEN_PLUS);
        } else if (token->type == TOKEN_MINUS) {
            eat(parser, TOKEN_MINUS);
        }

        ASTNode* new_node = (ASTNode*)malloc(sizeof(ASTNode));
        new_node->type = AST_BINOP;
        new_node->token = token;
        new_node->left = node;
        new_node->right = factor(parser);
        node = new_node;
    }

    return node;
}

void free_ast(ASTNode* node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node->token);
    free(node);
}

void free_parser(Parser* parser) {
    free(parser->current_token);
    free(parser);
}