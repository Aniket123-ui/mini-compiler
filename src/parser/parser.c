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
    if (parser->current_token) {
        fprintf(stderr, "[DEBUG] advance: token type=%d, value=%s\n", parser->current_token->type, parser->current_token->value ? parser->current_token->value : "(null)");
    }
}

// Forward declarations
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_term(Parser* parser);
static ASTNode* parse_additive(Parser* parser);
static ASTNode* parse_factor(Parser* parser);
static ASTNode* parse_declaration(Parser* parser);
static ASTNode* parse_assignment(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_block(Parser* parser);
static ASTNode* parse_if(Parser* parser);
static ASTNode* parse_while(Parser* parser);
static ASTNode* parse_comparison(Parser* parser);

// Use the exact create_*_node signatures from ast.c
// All create_*_node functions accept token argument if needed, or just name + ASTNode*

// --- Parser functions ---

Parser* create_parser(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;

    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    if (parser->current_token) {
        fprintf(stderr, "[DEBUG] create_parser: first token type=%d, value=%s\n", parser->current_token->type, parser->current_token->value ? parser->current_token->value : "(null)");
    }
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
    fprintf(stderr, "[DEBUG] parse_factor: token type=%d, value=%s\n", tok->type, tok->value ? tok->value : "(null)");
    if (tok->type == TOKEN_NUMBER) {
        int value = atoi(tok->value);
        advance(parser);
        return create_number_node(value, tok);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        char* name = strdup(tok->value);
        advance(parser);
        return create_identifier_node(name, tok);
    } else if (tok->type == TOKEN_LPAREN) {
        advance(parser);
        ASTNode* expr = parse_expression(parser);
        if (!expr) return NULL;
        if (!parser->current_token || parser->current_token->type != TOKEN_RPAREN) {
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
           (parser->current_token->type == TOKEN_MUL || parser->current_token->type == TOKEN_DIV)) {
        BinOpType op = (parser->current_token->type == TOKEN_MUL) ? OP_MUL : OP_DIV;
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

static ASTNode* parse_additive(Parser* parser) {
    ASTNode* node = parse_term(parser);
    if (!node) return NULL;
    while (parser->current_token &&
           (parser->current_token->type == TOKEN_PLUS || parser->current_token->type == TOKEN_MINUS)) {
        BinOpType op = (parser->current_token->type == TOKEN_PLUS) ? OP_ADD : OP_SUB;
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

static ASTNode* parse_comparison(Parser* parser) {
    ASTNode* node = parse_additive(parser);
    if (!node) return NULL;
    while (parser->current_token && (
        parser->current_token->type == TOKEN_LT ||
        parser->current_token->type == TOKEN_GT ||
        parser->current_token->type == TOKEN_LE ||
        parser->current_token->type == TOKEN_GE ||
        parser->current_token->type == TOKEN_EQ ||
        parser->current_token->type == TOKEN_NEQ)) {
        BinOpType op;
        switch (parser->current_token->type) {
            case TOKEN_LT: op = OP_LT; break;
            case TOKEN_GT: op = OP_GT; break;
            case TOKEN_LE: op = OP_LE; break;
            case TOKEN_GE: op = OP_GE; break;
            case TOKEN_EQ: op = OP_EQ; break;
            case TOKEN_NEQ: op = OP_NEQ; break;
            default: op = OP_ADD; break; // fallback
        }
        Token* tok = parser->current_token;
        advance(parser);
        ASTNode* right = parse_additive(parser);
        if (!right) {
            free_ast(node);
            return NULL;
        }
        node = create_binop_node(op, node, right, tok);
    }
    return node;
}

static ASTNode* parse_expression(Parser* parser) {
    return parse_comparison(parser);
}

static ASTNode* parse_declaration(Parser* parser) {
    // Expect: int identifier [= expr] ;
    if (parser->current_token->type != TOKEN_INT) {
        fprintf(stderr, "Expected 'int' keyword in declaration\n");
        return NULL;
    }
    Token* type_token = parser->current_token;
    advance(parser); // consume 'int'
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier after 'int'\n");
        return NULL;
    }
    char* name = strdup(parser->current_token->value);
    Token* id_token = parser->current_token;
    advance(parser); // consume identifier
    ASTNode* init_expr = NULL;
    if (parser->current_token && parser->current_token->type == TOKEN_ASSIGN) {
        advance(parser); // consume '='
        init_expr = parse_expression(parser);
        if (!init_expr) {
            free(name);
            return NULL;
        }
    }
    return create_declaration_node(name, init_expr, id_token);
}

static ASTNode* parse_assignment(Parser* parser) {
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier in assignment\n");
        return NULL;
    }
    char* name = strdup(parser->current_token->value);
    Token* tok = parser->current_token;
    advance(parser);
    if (!parser->current_token || parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Expected '=' in assignment\n");
        free(name);
        return NULL;
    }
    advance(parser);
    fprintf(stderr, "[DEBUG] parse_assignment: before parse_expression, token type=%d, value=%s\n", parser->current_token ? parser->current_token->type : -1, parser->current_token && parser->current_token->value ? parser->current_token->value : "(null)");
    ASTNode* expr = parse_expression(parser);
    fprintf(stderr, "[DEBUG] parse_assignment: after parse_expression, token type=%d, value=%s\n", parser->current_token ? parser->current_token->type : -1, parser->current_token && parser->current_token->value ? parser->current_token->value : "(null)");
    if (!expr) {
        free(name);
        return NULL;
    }
    return create_assignment_node(name, expr, tok);
}

static ASTNode* parse_if(Parser* parser) {
    // Expect: if (condition) then_branch [else else_branch]
    Token* tok = parser->current_token;
    advance(parser); // consume 'if'
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '(' after 'if'\n");
        return NULL;
    }
    advance(parser); // consume '('
    ASTNode* condition = parse_expression(parser);
    if (!condition) return NULL;
    if (!parser->current_token || parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ')' after if condition\n");
        free_ast(condition);
        return NULL;
    }
    advance(parser); // consume ')'
    ASTNode* then_branch = parse_statement(parser);
    ASTNode* else_branch = NULL;
    if (parser->current_token && parser->current_token->type == TOKEN_ELSE) {
        advance(parser); // consume 'else'
        else_branch = parse_statement(parser);
    }
    return create_if_node(condition, then_branch, else_branch, tok);
}

static ASTNode* parse_while(Parser* parser) {
    // Expect: while (condition) body
    Token* tok = parser->current_token;
    advance(parser); // consume 'while'
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '(' after 'while'\n");
        return NULL;
    }
    advance(parser); // consume '('
    ASTNode* condition = parse_expression(parser);
    if (!condition) return NULL;
    if (!parser->current_token || parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ')' after while condition\n");
        free_ast(condition);
        return NULL;
    }
    advance(parser); // consume ')'
    ASTNode* body = parse_statement(parser);
    return create_while_node(condition, body, tok);
}

static ASTNode* parse_statement(Parser* parser) {
    if (!parser->current_token) return NULL;
    fprintf(stderr, "[DEBUG] parse_statement: token type=%d, value=%s\n", parser->current_token->type, parser->current_token->value ? parser->current_token->value : "(null)");
    ASTNode* stmt = NULL;
    if (parser->current_token->type == TOKEN_LBRACE) {
        return parse_block(parser);
    } else if (parser->current_token->type == TOKEN_INT) {
        stmt = parse_declaration(parser);
    } else if (parser->current_token->type == TOKEN_IF) {
        stmt = parse_if(parser);
        return stmt;
    } else if (parser->current_token->type == TOKEN_WHILE) {
        stmt = parse_while(parser);
        return stmt;
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        Token* next = lexer_peek_token(parser->lexer);
        if (next && next->type == TOKEN_ASSIGN) {
            stmt = parse_assignment(parser);
        } else {
            fprintf(stderr, "Error: Only assignments are supported for identifiers.\n");
            return NULL;
        }
    } else {
        stmt = parse_expression(parser);
    }
    // After parsing a statement, expect a semicolon
    if (parser->current_token && parser->current_token->type == TOKEN_SEMICOLON) {
        advance(parser);
    } else {
        fprintf(stderr, "Error: Expected ';' after statement\n");
        if (stmt) free_ast(stmt);
        return NULL;
    }
    return stmt;
}

static ASTNode* parse_block(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_LBRACE) {
        fprintf(stderr, "Expected '{' to start block\n");
        return NULL;
    }
    advance(parser);

    ASTNode** statements = NULL;
    size_t capacity = 4;
    size_t count = 0;
    statements = malloc(sizeof(ASTNode*) * capacity);

    while (parser->current_token && parser->current_token->type != TOKEN_RBRACE) {
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

    if (!parser->current_token || parser->current_token->type != TOKEN_RBRACE) {
        fprintf(stderr, "Expected '}' to close block\n");
        for (size_t i = 0; i < count; i++) free_ast(statements[i]);
        free(statements);
        return NULL;
    }
    advance(parser);

    ASTNode* compound = create_compound_node(statements, count);
    free(statements);
    return compound;
}

ASTNode* parse(Parser* parser) {
    ASTNode** statements = NULL;
    size_t capacity = 4, count = 0;
    statements = malloc(sizeof(ASTNode*) * capacity);

    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        fprintf(stderr, "[DEBUG] parse: token type=%d, value=%s\n", parser->current_token->type, parser->current_token->value ? parser->current_token->value : "(null)");
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
    fprintf(stderr, "[DEBUG] parse: returning compound node with %zu statements\n", count);
    return compound;
}
