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

bool FunDecl::typecheck(Environment& env, FuncEnvironment& fenv) const {

    if (fenv.find(fname) != fenv.end())
        return false;

    fenv[fname] = {frtype.get()};

    for (auto& arg: args)
        fenv[fname].push_back(arg.first);

    for (auto& s: block) {
        if (s->typecheck(env, this->fname, fenv) == false)
            return false;
    }

    return true;
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

bool GlobalDecl::typecheck(Environment& env, FuncEnvironment& fenv) const {

    const char* global_env = "__global__";

    if (env.find(global_env) == env.end())
        env[global_env] = {};

    if (env[global_env].find(id) != env[global_env].end())
        return false;

    auto exp_ty = exp->typecheck(env, global_env, fenv);

    if (exp_ty == nullptr)
        return false;

    if (!((*exp_ty) == (*(ty.get()))))
        return false;

    env[global_env][id] = ty.get();

    return true;
}
