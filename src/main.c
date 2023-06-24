#include "../inc/helpers.h"
#include "../inc/lexer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_error("Invalid number of arguments", 1);
    }

    char *file_dest = argv[1];
    lex_file(file_dest);

    exit(EXIT_SUCCESS);
}
