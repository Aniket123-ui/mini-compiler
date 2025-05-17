#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void eat(Parser* parser, TokenType expected) {
    if (parser->current_token->type == expected) {
        free_token(parser->current_token);
        parser->current_token = get_next_token(parser->lexer);
    } else {
        printf("Parser Error: Expected token %d but got %d at line %d column %d\n",
            expected, parser->current_token->type,
            parser->current_token->line, parser->current_token->column);
        exit(1);
    }
}

Parser* create_parser(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = get_next_token(lexer);
    return parser;
}

// Forward declarations
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_block(Parser* parser);

static ASTNode* parse_factor(Parser* parser) {
    Token* tok = parser->current_token;
    if (tok->type == TOKEN_NUMBER) {
        eat(parser, TOKEN_NUMBER);
        return create_number_node(atoi(tok->value), tok);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        eat(parser, TOKEN_IDENTIFIER);
        return create_identifier_node(strdup(tok->value), tok);
    } else if (tok->type == TOKEN_LPAREN) {
        eat(parser, TOKEN_LPAREN);
        ASTNode* expr = parse_expression(parser);
        eat(parser, TOKEN_RPAREN);
        return expr;
    } else {
        printf("Parser Error: Invalid factor at line %d column %d\n", tok->line, tok->column);
        exit(1);
    }
}

static ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    while (parser->current_token->type == TOKEN_MUL ||
           parser->current_token->type == TOKEN_DIV) {
        Token* tok = parser->current_token;
        OperatorType op = (tok->type == TOKEN_MUL) ? OP_MUL : OP_DIV;
        eat(parser, tok->type);
        node = create_binop_node(op, node, parse_factor(parser), tok);
    }
    return node;
}

static ASTNode* parse_expression(Parser* parser) {
    ASTNode* node = parse_term(parser);
    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS) {
        Token* tok = parser->current_token;
        OperatorType op = (tok->type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
        eat(parser, tok->type);
        node = create_binop_node(op, node, parse_term(parser), tok);
    }
    return node;
}

static ASTNode* parse_declaration(Parser* parser) {
    eat(parser, TOKEN_INT); // consume 'int'
    Token* id_tok = parser->current_token;
    if (id_tok->type != TOKEN_IDENTIFIER) {
        printf("Parser Error: Expected identifier after 'int' at line %d\n", id_tok->line);
        exit(1);
    }
    char* name = strdup(id_tok->value);
    eat(parser, TOKEN_IDENTIFIER);

    ASTNode* expr = NULL;
    if (parser->current_token->type == TOKEN_ASSIGN) {
        eat(parser, TOKEN_ASSIGN);
        expr = parse_expression(parser);
    }
    eat(parser, TOKEN_SEMICOLON);
    return create_declaration_node(name, expr, id_tok);
}

static ASTNode* parse_assignment(Parser* parser) {
    Token* id_tok = parser->current_token;
    ASTNode* id_node = create_identifier_node(strdup(id_tok->value), id_tok);
    eat(parser, TOKEN_IDENTIFIER);

    eat(parser, TOKEN_ASSIGN);
    ASTNode* expr = parse_expression(parser);
    eat(parser, TOKEN_SEMICOLON);
    return create_assignment_node(id_node, expr, id_tok);
}

static ASTNode* parse_if_statement(Parser* parser) {
    eat(parser, TOKEN_IF);
    eat(parser, TOKEN_LPAREN);
    ASTNode* cond = parse_expression(parser);
    eat(parser, TOKEN_RPAREN);

    ASTNode* then_stmt = parse_statement(parser);
    ASTNode* else_stmt = NULL;

    if (parser->current_token->type == TOKEN_ELSE) {
        eat(parser, TOKEN_ELSE);
        else_stmt = parse_statement(parser);
    }

    return create_if_node(cond, then_stmt, else_stmt);
}

static ASTNode* parse_while_statement(Parser* parser) {
    eat(parser, TOKEN_WHILE);
    eat(parser, TOKEN_LPAREN);
    ASTNode* cond = parse_expression(parser);
    eat(parser, TOKEN_RPAREN);

    ASTNode* body = parse_statement(parser);
    return create_while_node(cond, body);
}

static ASTNode* parse_return_statement(Parser* parser) {
    eat(parser, TOKEN_RETURN);
    ASTNode* expr = parse_expression(parser);
    eat(parser, TOKEN_SEMICOLON);
    return create_return_node(expr);
}

static ASTNode* parse_block(Parser* parser) {
    eat(parser, TOKEN_LBRACE);
    ASTNode* block = create_compound_node();

    while (parser->current_token->type != TOKEN_RBRACE) {
        ASTNode* stmt = parse_statement(parser);
        add_statement_to_block(block, stmt);
    }

    eat(parser, TOKEN_RBRACE);
    return block;
}

static ASTNode* parse_statement(Parser* parser) {
    switch (parser->current_token->type) {
        case TOKEN_INT:
            return parse_declaration(parser);
        case TOKEN_IDENTIFIER:
            return parse_assignment(parser);
        case TOKEN_IF:
            return parse_if_statement(parser);
        case TOKEN_WHILE:
            return parse_while_statement(parser);
        case TOKEN_RETURN:
            return parse_return_statement(parser);
        case TOKEN_LBRACE:
            return parse_block(parser);
        default:
            printf("Parser Error: Unexpected token %d at line %d\n", 
                   parser->current_token->type, parser->current_token->line);
            exit(1);
    }
}

ASTNode* parse(Parser* parser) {
    return parse_block(parser);
}

void free_parser(Parser* parser) {
    free_token(parser->current_token);
    free(parser);
}
