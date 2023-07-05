#include "../inc/helpers.h"
#include "../inc/lexer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_error(ERR_NUM_ARGS, "Invalid number of arguments : %d", NULL,
                    argc);
    }

    lex_and_parse(argv[1]);

    exit(EXIT_SUCCESS);
}
