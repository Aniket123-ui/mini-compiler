#include "interpreter.h"

#define MAX_INPUT 1024

int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("calc> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) {
            continue;
        }

        Lexer* lexer = create_lexer(input);
        Parser* parser = create_parser(lexer);
        Interpreter* interpreter = create_interpreter(parser);

        int result = interpret(interpreter);
        printf("%d\n", result);

        free_interpreter(interpreter);
        free_parser(parser);
        free_lexer(lexer);
    }

    return 0;
}