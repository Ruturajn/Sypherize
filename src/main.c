#include "../inc/helpers.h"
#include "../inc/lexer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_error("Invalid number of arguments", 1, NULL);
    }

    lex_and_parse(argv[1]);

    exit(EXIT_SUCCESS);
}
