
```flex:c%3A%5CUsers%5Cchauh%5Cmini%20compiler%5Clexer.l
%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "error_handler.h"

int line_number = 1;
int column = 1;
%}

%%
[ \t]           { column += yyleng; }
[\n]            { line_number++; column = 1; }

"int"           { column += yyleng; return INT; }
"if"            { column += yyleng; return IF; }
"else"          { column += yyleng; return ELSE; }
"while"         { column += yyleng; return WHILE; }
"return"        { column += yyleng; return RETURN; }

[a-zA-Z_][a-zA-Z0-9_]*  { 
    column += yyleng;
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

[0-9]+          {
    column += yyleng;
    yylval.number = atoi(yytext);
    return NUMBER;
}

"+"             { column += yyleng; return '+'; }
"-"             { column += yyleng; return '-'; }
"*"             { column += yyleng; return '*'; }
"/"             { column += yyleng; return '/'; }
"="             { column += yyleng; return '='; }
"=="            { column += yyleng; return EQ; }
"!="            { column += yyleng; return NE; }
"<"             { column += yyleng; return '<'; }
">"             { column += yyleng; return '>'; }
"<="            { column += yyleng; return LE; }
">="            { column += yyleng; return GE; }

"{"             { column += yyleng; return '{'; }
"}"             { column += yyleng; return '}'; }
"("             { column += yyleng; return '('; }
")"             { column += yyleng; return ')'; }
";"             { column += yyleng; return ';'; }

.               { 
    report_error(line_number, column, "Invalid character: %s", yytext);
}

%%

int yywrap() {
    return 1;
}%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "error_handler.h"

extern int line_number;
extern int column;
extern int yylex();
void yyerror(const char *s);

ASTNode *root = NULL;
%}

%union {
    int number;
    char *string;
    struct ASTNode *node;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token INT IF ELSE WHILE RETURN
%token EQ NE LE GE

%type <node> program declaration statement expression
%type <node> compound_statement statement_list

%%

program
    : declaration                { root = $1; }
    ;

declaration
    : INT IDENTIFIER '(' ')' compound_statement
        {
            $$ = create_function_node($2, $5);
            free($2);
        }
    ;

compound_statement
    : '{' statement_list '}'    { $$ = $2; }
    | '{' '}'                   { $$ = create_compound_statement(NULL); }
    ;

statement_list
    : statement                 { $$ = create_compound_statement($1); }
    | statement_list statement  { $$ = add_statement($1, $2); }
    ;

statement
    : expression ';'            { $$ = $1; }
    | INT IDENTIFIER ';'        
        {
            $$ = create_var_declaration($2);
            free($2);
        }
    | IDENTIFIER '=' expression ';'
        {
            $$ = create_assignment($1, $3);
            free($1);
        }
    | IF '(' expression ')' statement
        { $$ = create_if_statement($3, $5, NULL); }
    | IF '(' expression ')' statement ELSE statement
        { $$ = create_if_statement($3, $5, $7); }
    | WHILE '(' expression ')' statement
        { $$ = create_while_statement($3, $5); }
    | RETURN expression ';'     { $$ = create_return_statement($2); }
    | compound_statement        { $$ = $1; }
    ;

expression
    : NUMBER                    { $$ = create_number_node($1); }
    | IDENTIFIER               
        {
            $$ = create_identifier_node($1);
            free($1);
        }
    | expression '+' expression { $$ = create_binary_op('+', $1, $3); }
    | expression '-' expression { $$ = create_binary_op('-', $1, $3); }
    | expression '*' expression { $$ = create_binary_op('*', $1, $3); }
    | expression '/' expression { $$ = create_binary_op('/', $1, $3); }
    | expression EQ expression  { $$ = create_binary_op(EQ, $1, $3); }
    | expression NE expression  { $$ = create_binary_op(NE, $1, $3); }
    | expression '<' expression { $$ = create_binary_op('<', $1, $3); }
    | expression '>' expression { $$ = create_binary_op('>', $1, $3); }
    | expression LE expression  { $$ = create_binary_op(LE, $1, $3); }
    | expression GE expression  { $$ = create_binary_op(GE, $1, $3); }
    | '(' expression ')'        { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    report_error(line_number, column, "%s", s);
}