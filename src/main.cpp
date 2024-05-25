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

    std::ifstream input_file;
    input_file.open(file_name);

    if (!input_file.is_open()) {
        std::cerr << "Invalid: file path!\n";
        std::cerr << "  Usage: " << program << " <FILE_PATH>\n";
        return -1;
    }

    std::string file_data((std::istreambuf_iterator<char>(input_file)),
                          std::istreambuf_iterator<char>());

    input_file.close();

    // Lex
    Lexer lexer(file_data);
    lexer.lex();
    if (lexer.failed)
        return -1;

    lexer.print_tokens();

    // Parse
    Parser parser(lexer.tok_list, file_data, file_name);
    parser.parse_prog();
    if (parser.failed)
        return -1;

    parser.prog.print_prog();

    // Typecheck
    TypeChecker tc(&parser.prog);
    if (!tc.typecheck())
        return -1;

    return 0;
}
