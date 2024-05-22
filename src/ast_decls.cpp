#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// FunDecl
///===-------------------------------------------------------------------===///

void FunDecl::print_decl(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "FUN_DECL:\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNNAME: " << fname << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNARGS:\n";

    for (auto &arg: args) {

        for (int i = 0; i < indent + 8; i++)
            std::cout << " ";

        std::cout << "ARG:";
        arg.first->print_type();
        std::cout << arg.second << "\n";
    }

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNBODY:\n";

    for (auto &s: block)
        s->print_stmt(indent + 8);
}

///===-------------------------------------------------------------------===///
/// GlobalDecl
///===-------------------------------------------------------------------===///

void GlobalDecl::print_decl(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "GLOBAL_DECL:\n";

    for (int i = 0; i < indent + 8; i++)
        std::cout << " ";

    std::cout << "TYPE: ";
    if (ty != nullptr)
        ty->print_type();
    else
        std::cout << "Array";
    std::cout << "\n";

    for (int i = 0; i < indent + 8; i++)
        std::cout << " ";

    std::cout << "ID: " << id << "\n";
    for (int i = 0; i < indent + 8; i++)
        std::cout << " ";

    std::cout << "EXP:\n";
    exp->print_node(indent + 12);
}
