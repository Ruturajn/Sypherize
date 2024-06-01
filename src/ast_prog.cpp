#include "../inc/ast.h"

using namespace AST;

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///

void Program::print_prog() const {
    std::cout << "PROG:\n";

    for (auto &d: decl_list)
        d->print_decl(4);
}

bool Program::typecheck(Environment& env, FuncEnvironment& fenv,
                        Diagnostics* diag) const {
    for (auto& decl: decl_list) {
        if (decl->typecheck(env, fenv, diag) == false)
            return false;
    }

    return true;
}

bool Program::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                      LLProg& llprog) const {
    for (auto d : decl_list) {
        if (d->compile(ctxt, out, diag, llprog) == false) {
            diag->print_error(d->sr, "[ICE] Unable to compile top-level declaration");
            return false;
        }
    }

    return true;
}
