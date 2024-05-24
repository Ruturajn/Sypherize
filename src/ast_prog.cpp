#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///

void Program::print_prog() const {
    std::cout << "PROG:\n";

    for (auto &d: decl_list)
        d->print_decl(4);
}

bool Program::typecheck(Environment& env, FuncEnvironment& fenv) const {
    for (auto& decl: decl_list) {
        if (decl->typecheck(env, fenv) == false)
            return false;
    }

    return true;
}
