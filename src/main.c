#include <stdio.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "parser/ast.h"
// For analyze_semantics

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    Lexer* lexer = create_lexer_from_file(argv[1]);
    if (!lexer) {
        fprintf(stderr, "Failed to open input file.\n");
        return 1;
    }

    Parser* parser = create_parser(lexer);
    if (!parser) {
        fprintf(stderr, "Failed to create parser.\n");
        free_lexer(lexer);
        return 1;
    }

    ASTNode* ast_root = parse(parser);
    if (!ast_root) {
        fprintf(stderr, "Parsing failed.\n");
        free_parser(parser);
        free_lexer(lexer);
        return 1;
    }

    analyze_semantics(ast_root);

    // TODO: Code generation here

    free_ast(ast_root);
    free_parser(parser);
    free_lexer(lexer);

    return 0;
}
