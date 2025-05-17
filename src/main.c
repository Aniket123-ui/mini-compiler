#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "ir/optimizer.h"
#include "ir/ir_generator.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* input_file = fopen("input.txt", "r");
    if (!input_file) {
        fprintf(stderr, "Error: Could not open input.txt\n");
        return 1;
    }

    // Read entire file into string
    fseek(input_file, 0, SEEK_END);
    long length = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    char* source_code = malloc(length + 1);
    fread(source_code, 1, length, input_file);
    source_code[length] = '\0';
    fclose(input_file);

    // Initialize lexer and parser
    Lexer* lexer = create_lexer(source_code);
    Parser* parser = create_parser(lexer);

    // Parse the input into an AST
    ASTNode* ast_root = parse(parser);

    if (!ast_root) {
        fprintf(stderr, "Parsing failed. AST is NULL.\n");
        return 1;
    }

    // Semantic Analysis
    analyze_semantics(ast_root);

    // Optimization
    optimize_ir(ast_root);

    // Intermediate Code Generation
    FILE* ir_output = fopen("output.ir", "w");
    if (!ir_output) {
        fprintf(stderr, "Error: Could not open output.ir\n");
        return 1;
    }

    generate_ir(ast_root, ir_output);
    fclose(ir_output);

    printf("Compilation successful. IR written to output.ir\n");

    // Clean up
    free(source_code);
    free_parser(parser);
    free_ast(ast_root);

    return 0;
}
