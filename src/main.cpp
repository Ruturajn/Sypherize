#include <iostream>
#include "../inc/lexer.h"

int main(int argc, char* argv[]) {

    Lexer lexer(":@ #");
    lexer.print_tokens();

    return 0;
}
