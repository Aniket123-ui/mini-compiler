#include "parser.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#define strdup _strdup
#endif

static void advance(Parser* parser) {
    parser->current_token = lexer_next_token(parser->lexer);
}

// Forward declarations
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_declaration(Parser* parser);
static ASTNode* parse_assignment(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_block(Parser* parser);

// Use the exact create_*_node signatures from ast.c
// All create_*_node functions accept token argument if needed, or just name + ASTNode*

// --- Parser functions ---

Parser* create_parser(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;

    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void free_parser(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

static ASTNode* parse_factor(Parser* parser) {
    Token* tok = parser->current_token;
    if (!tok) return NULL;

    if (tok->type == TOKEN_NUMBER) {
        int value = atoi(tok->value);
        advance(parser);
        return create_number_node(value, tok);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        char* name = strdup(tok->value);
        advance(parser);
        return create_identifier_node(name, tok);
    } else if (tok->type == TOKEN_PUNCTUATION && strcmp(tok->value, "(") == 0) {
        advance(parser);
        ASTNode* expr = parse_expression(parser);
        if (!expr) return NULL;
        if (!parser->current_token || parser->current_token->type != TOKEN_PUNCTUATION || strcmp(parser->current_token->value, ")") != 0) {
            fprintf(stderr, "Error: Expected ')'\n");
            free_ast(expr);
            return NULL;
        }
        advance(parser);
        return expr;
    }

    fprintf(stderr, "Error: Unexpected token in factor\n");
    return NULL;
}

static ASTNode* parse_term(Parser* parser) {
    ASTNode* node = parse_factor(parser);
    if (!node) return NULL;

    while (parser->current_token &&
           parser->current_token->type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token->value, "*") == 0 || strcmp(parser->current_token->value, "/") == 0)) {
        BinOpType op = (strcmp(parser->current_token->value, "*") == 0) ? OP_MUL : OP_DIV;
        Token* tok = parser->current_token;
        advance(parser);
        ASTNode* right = parse_factor(parser);
        if (!right) {
            free_ast(node);
            return NULL;
        }
        node = create_binop_node(op, node, right, tok);
    }
    return node;
}

static ASTNode* parse_expression(Parser* parser) {
    ASTNode* node = parse_term(parser);
    if (!node) return NULL;

    while (parser->current_token &&
           parser->current_token->type == TOKEN_OPERATOR &&
           (strcmp(parser->current_token->value, "+") == 0 || strcmp(parser->current_token->value, "-") == 0)) {
        BinOpType op = (strcmp(parser->current_token->value, "+") == 0) ? OP_ADD : OP_SUB;
        Token* tok = parser->current_token;
        advance(parser);
        ASTNode* right = parse_term(parser);
        if (!right) {
            free_ast(node);
            return NULL;
        }
        node = create_binop_node(op, node, right, tok);
    }
    return node;
}

static ASTNode* parse_declaration(Parser* parser) {
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier in declaration\n");
        return NULL;
    }
    char* name = strdup(parser->current_token->value);
    Token* tok = parser->current_token;
    advance(parser);

    ASTNode* init_expr = NULL;
    if (parser->current_token && strcmp(parser->current_token->value, "=") == 0) {
        advance(parser);
        init_expr = parse_expression(parser);
        if (!init_expr) {
            free(name);
            return NULL;
        }
    }
    return create_declaration_node(name, init_expr, tok);
}

static ASTNode* parse_assignment(Parser* parser) {
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier in assignment\n");
        return NULL;
    }
    char* name = strdup(parser->current_token->value);
    Token* tok = parser->current_token;
    advance(parser);

    if (!parser->current_token || strcmp(parser->current_token->value, "=") != 0) {
        fprintf(stderr, "Expected '=' in assignment\n");
        free(name);
        return NULL;
    }
    advance(parser);
    ASTNode* expr = parse_expression(parser);
    if (!expr) {
        free(name);
        return NULL;
    }
    return create_assignment_node(create_identifier_node(name, tok), expr, tok);
}

static ASTNode* parse_statement(Parser* parser) {
    if (!parser->current_token) return NULL;

    if (strcmp(parser->current_token->value, "{") == 0) {
        return parse_block(parser);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        Token* next = lexer_peek_token(parser->lexer);
        if (next && strcmp(next->value, "=") == 0) {
            return parse_assignment(parser);
        } else {
            return parse_declaration(parser);
        }
    } else {
        return parse_expression(parser);
    }
}

static ASTNode* parse_block(Parser* parser) {
    if (!parser->current_token || strcmp(parser->current_token->value, "{") != 0) {
        fprintf(stderr, "Expected '{' to start block\n");
        return NULL;
    }
    advance(parser);

    ASTNode** statements = NULL;
    size_t capacity = 4;
    size_t count = 0;
    statements = malloc(sizeof(ASTNode*) * capacity);

    while (parser->current_token && strcmp(parser->current_token->value, "}") != 0) {
        ASTNode* stmt = parse_statement(parser);
        if (!stmt) {
            for (size_t i = 0; i < count; ++i) free_ast(statements[i]);
            free(statements);
            return NULL;
        }
        if (count >= capacity) {
            capacity *= 2;
            ASTNode** new_stmts = realloc(statements, sizeof(ASTNode*) * capacity);
            if (!new_stmts) {
                for (size_t i = 0; i < count; ++i) free_ast(statements[i]);
                free(statements);
                return NULL;
            }
            statements = new_stmts;
        }
        statements[count++] = stmt;
    }

    if (!parser->current_token || strcmp(parser->current_token->value, "}") != 0) {
        fprintf(stderr, "Expected '}' to close block\n");
        for (size_t i = 0; i < count; ++i) free_ast(statements[i]);
        free(statements);
        return NULL;
    }
    advance(parser);

    ASTNode* compound = create_compound_node(statements, count);
    // compound node owns statements now, so free array only
    free(statements);
    return compound;
}

ASTNode* parse(Parser* parser) {
    ASTNode** statements = NULL;
    size_t capacity = 4, count = 0;
    statements = malloc(sizeof(ASTNode*) * capacity);

    while (parser->current_token) {
        ASTNode* stmt = parse_statement(parser);
        if (!stmt) {
            for (size_t i = 0; i < count; ++i) free_ast(statements[i]);
            free(statements);
            return NULL;
        }
        if (count >= capacity) {
            capacity *= 2;
            ASTNode** new_stmts = realloc(statements, sizeof(ASTNode*) * capacity);
            if (!new_stmts) {
                for (size_t i = 0; i < count; ++i) free_ast(statements[i]);
                free(statements);
                return NULL;
            }
            statements = new_stmts;
        }
        statements[count++] = stmt;
    }

    ASTNode* compound = create_compound_node(statements, count);
    free(statements);
    return compound;
}
