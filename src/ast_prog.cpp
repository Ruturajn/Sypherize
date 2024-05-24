#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///

void Program::print_prog() const {
    std::cout << "PROG:\n";

    for (auto &d: decl_list)
        d->print_decl(4);
}

bool Program::typecheck() const {
    for (auto &d: decl_list) {
        if (d->typecheck_decl() == false)
            return false;
    }

    return true;
}
