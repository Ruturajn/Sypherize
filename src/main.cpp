#include <fstream>
#include <iostream>
#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/typechecker.h"

int main(int argc, char* argv[]) {

    char *program = *argv++;

    if (argc <= 1) {
        std::cerr << "Invalid: file path not provided!\n";
        std::cerr << "  Usage: " << program << " <FILE_PATH>\n";
        return -1;
    }

    char *file_name = *argv++;

    Diagnostics diag(file_name);
    diag.read_file(program);

    // Lex
    Lexer lexer(diag.file_data);
    lexer.lex();
    if (lexer.failed)
        return -1;

    lexer.print_tokens();

    // Parse
    Parser parser(lexer.tok_list, &diag);
    parser.parse_prog();
    if (parser.failed)
        return -1;

    parser.prog.print_prog();

    // Typecheck
    TypeChecker tc(&parser.prog);
    if (!tc.typecheck(&diag))
        return -1;

    return 0;
}
