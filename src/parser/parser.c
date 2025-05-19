#include "parser.h"
#include "ast.h"
#include "../utils/symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#define strdup _strdup
#endif

static void advance(Parser* parser) {
    parser->current_token = lexer_next_token(parser->lexer);
    if (parser->current_token) {
        fprintf(stderr, "[DEBUG] advance: token type=%d, value=%s\n",
            parser->current_token->type,
            parser->current_token->value ? parser->current_token->value : "(null)");
    }
}

// Grammar rules for C-like language:
//
// program           : function_definition
// function_definition: type IDENTIFIER '(' ')' compound_statement
// block             : '{' { statement } '}'
// statement         : declaration | assignment | if_statement | while_statement | return_statement | block
// declaration       : 'int' IDENTIFIER [ '=' expression ] ';'
// assignment        : IDENTIFIER '=' expression ';'
// if_statement      : 'if' '(' expression ')' statement [ 'else' statement ]
// while_statement   : 'while' '(' expression ')' statement
// return_statement  : 'return' expression ';'
// expression        : ... (handled by parse_expression)

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
static ASTNode* parse_function_definition(Parser* parser, const char* name);

// Parser symbol table
// static SymbolTable* parser_symbol_table = NULL;

Parser* create_parser(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    if (parser->current_token) {
        fprintf(stderr, "[DEBUG] create_parser: first token type=%d, value=%s\n",
            parser->current_token->type,
            parser->current_token->value ? parser->current_token->value : "(null)");
    }
    // No local parser_symbol_table, rely on global symbol_table
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
    // Prevent return from being parsed as a factor
    if (tok->type == TOKEN_RETURN) {
        return NULL;
    }
    fprintf(stderr, "[DEBUG] parse_factor: token type=%d, value=%s\n",
        tok->type, tok->value ? tok->value : "(null)");

    if (tok->type == TOKEN_NUMBER) {
        int value = atoi(tok->value);
        advance(parser);
        return create_number_node(value, tok);
    } else if (tok->type == TOKEN_IDENTIFIER) {
        char* name = strdup(tok->value);
        // Check if identifier is declared
        if (!lookup_symbol(symbol_table, name)) {
            fprintf(stderr, "[ERROR] Variable '%s' used before declaration (parse_factor)\n", name);
            free(name);
            return NULL;
        }
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
    // Add variable to global symbol table
    add_symbol(name);
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
    fprintf(stderr, "[DEBUG] parse_assignment: before parse_expression, token type=%d, value=%s\n",
        parser->current_token ? parser->current_token->type : -1,
        parser->current_token && parser->current_token->value ? parser->current_token->value : "(null)");
    ASTNode* expr = parse_expression(parser);
    fprintf(stderr, "[DEBUG] parse_assignment: after parse_expression, token type=%d, value=%s\n",
        parser->current_token ? parser->current_token->type : -1,
        parser->current_token && parser->current_token->value ? parser->current_token->value : "(null)");
    if (!expr) {
        free(name);
        return NULL;
    }
    return create_assignment_node(name, expr, tok);
}

static ASTNode* parse_if(Parser* parser) {
    // Expect: if '(' expr ')' statement [else statement]
    if (parser->current_token->type != TOKEN_IF) {
        fprintf(stderr, "Expected 'if'\n");
        return NULL;
    }
    Token* if_token = parser->current_token;
    advance(parser);
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '('\n");
        return NULL;
    }
    advance(parser);
    ASTNode* condition = parse_expression(parser);
    if (!condition) return NULL;
    if (!parser->current_token || parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ')'\n");
        free_ast(condition);
        return NULL;
    }
    advance(parser);
    ASTNode* then_branch = parse_statement(parser);
    if (!then_branch) {
        free_ast(condition);
        return NULL;
    }
    ASTNode* else_branch = NULL;
    if (parser->current_token && parser->current_token->type == TOKEN_ELSE) {
        advance(parser);
        else_branch = parse_statement(parser);
        if (!else_branch) {
            free_ast(condition);
            free_ast(then_branch);
            return NULL;
        }
    }
    return create_if_node(condition, then_branch, else_branch, if_token);
}

static ASTNode* parse_while(Parser* parser) {
    // Expect: while '(' expr ')' statement
    if (parser->current_token->type != TOKEN_WHILE) {
        fprintf(stderr, "Expected 'while'\n");
        return NULL;
    }
    Token* while_token = parser->current_token;
    advance(parser);
    if (!parser->current_token || parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '('\n");
        return NULL;
    }
    advance(parser);
    ASTNode* condition = parse_expression(parser);
    if (!condition) return NULL;
    if (!parser->current_token || parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ')'\n");
        free_ast(condition);
        return NULL;
    }
    advance(parser);
    ASTNode* body = parse_statement(parser);
    if (!body) {
        free_ast(condition);
        return NULL;
    }
    return create_while_node(condition, body, while_token);
}

static ASTNode* parse_return(Parser* parser) {
    if (parser->current_token->type != TOKEN_RETURN) {
        fprintf(stderr, "Expected 'return'\n");
        return NULL;
    }
    Token* ret_token = parser->current_token;
    advance(parser);
    ASTNode* expr = parse_expression(parser);
    if (!expr) return NULL;
    return create_return_node(expr, ret_token);
}

static ASTNode* parse_block(Parser* parser) {
    // Expect '{' { statement } '}'
    if (!parser->current_token || parser->current_token->type != TOKEN_LBRACE) {
        fprintf(stderr, "Expected '{'\n");
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
        fprintf(stderr, "Expected '}'\n");
        for (size_t i = 0; i < count; i++) free_ast(statements[i]);
        free(statements);
        return NULL;
    }
    advance(parser);

    ASTNode* block = create_compound_node(statements, count);
    free(statements);
    return block;
}

static ASTNode* parse_statement(Parser* parser) {
    if (!parser->current_token) return NULL;

    switch (parser->current_token->type) {
        case TOKEN_INT:
            {
                ASTNode* decl = parse_declaration(parser);
                if (!decl) return NULL;
                if (!parser->current_token || parser->current_token->type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Expected ';' after declaration\n");
                    free_ast(decl);
                    return NULL;
                }
                advance(parser);
                return decl;
            }
        case TOKEN_IDENTIFIER:
            {
                ASTNode* assign = parse_assignment(parser);
                if (!assign) return NULL;
                if (!parser->current_token || parser->current_token->type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Expected ';' after assignment\n");
                    free_ast(assign);
                    return NULL;
                }
                advance(parser);
                return assign;
            }
        case TOKEN_IF:
            return parse_if(parser);
        case TOKEN_WHILE:
            return parse_while(parser);
        case TOKEN_RETURN:
            {
                ASTNode* ret = parse_return(parser);
                if (!ret) return NULL;
                if (!parser->current_token || parser->current_token->type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Expected ';' after return\n");
                    free_ast(ret);
                    return NULL;
                }
                advance(parser);
                return ret;
            }
        case TOKEN_LBRACE:
            return parse_block(parser);
        default:
            fprintf(stderr, "Unexpected token in statement: %d\n", parser->current_token->type);
            return NULL;
    }
}

static ASTNode* parse_function_definition(Parser* parser, const char* name) {
    // Expect: int identifier '(' ')' block
    if (parser->current_token->type != TOKEN_INT) {
        fprintf(stderr, "Expected 'int' at function definition\n");
        return NULL;
    }
    Token* type_token = parser->current_token;
    advance(parser);

    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected function name\n");
        return NULL;
    }
    const char* func_name = parser->current_token->value;
    advance(parser);

    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Expected '('\n");
        return NULL;
    }
    advance(parser);

    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected ')'\n");
        return NULL;
    }
    advance(parser);

    ASTNode* body = parse_block(parser);
    if (!body) return NULL;

    return create_function_node(func_name, body, type_token);
}

ASTNode* parse_program(Parser* parser) {
    return parse_function_definition(parser, NULL);
}
