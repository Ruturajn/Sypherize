#include "../inc/ast.h"

using namespace AST;

///===-------------------------------------------------------------------===///
/// FunDecl
///===-------------------------------------------------------------------===///

void FunDecl::print_decl(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "FUN_DECL";
    sr.print_srange(std::cout);
    std::cout << ":\n";

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

bool FunDecl::typecheck(Environment& env, FuncEnvironment& fenv,
                        Diagnostics* diag) const {

    if (fenv.find(fname) != fenv.end()) {
        diag->print_error(sr, "Redefinition of function");
        return false;
    }

    if (env.find(fname) != env.end()) {
        diag->print_error(sr, "[ICE] Function local scope exists without it being"
                "present in fenv");
        return false;
    }

    fenv[fname] = {frtype.get()};

    for (auto& arg: args) {
        fenv[fname].push_back(arg.first);
        env[fname][arg.second] = arg.first;
    }

    for (auto& s: block) {
        if (s->typecheck(env, this->fname, fenv, diag) == false)
            return false;
    }

    return true;
}

bool FunDecl::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {
    LLCtxt fun_ctxt = ctxt;

    std::unique_ptr<LLType> fun_ret_ty(this->frtype->compile_type());
    std::vector<LLType*> arg_ty_list;

    for (auto& arg: this->args) {
        auto arg_uid = gentemp_ll(arg.second);
        fun_ctxt[arg.second] = {arg.first->compile_type(), new LLOId(arg_uid)};
        arg_ty_list.push_back(arg.first->compile_type());
    }

    std::unique_ptr<LLType> func_ty(new LLTFunc(arg_ty_list, std::move(fun_ret_ty)));

    fun_ctxt[this->fname] = {new LLTPtr(std::move(func_ty)), new LLOGid(this->fname)};

    for (auto s : block) {
        if (s->compile(fun_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }

    ctxt[this->fname] = {
        new LLTPtr(std::move(fun_ctxt[this->fname].first->clone())),
        new LLOGid(this->fname)
    };

    return true;
}

///===-------------------------------------------------------------------===///
/// GlobalDecl
///===-------------------------------------------------------------------===///

void GlobalDecl::print_decl(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "GLOBAL_DECL";
    sr.print_srange(std::cout);
    std::cout << ":\n";

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

bool GlobalDecl::typecheck(Environment& env, FuncEnvironment& fenv,
                           Diagnostics* diag) const {

    const char* global_env = "__global__";

    if (env.find(global_env) == env.end())
        env[global_env] = {};

    if (env[global_env].find(id) != env[global_env].end()) {
        diag->print_error(sr, "Redefinition of global variable");
        return false;
    }

    auto exp_ty = exp->typecheck(env, global_env, fenv, diag);

    if (exp_ty == nullptr)
        return false;

    if (!((*exp_ty) == (*(ty.get())))) {
        std::string err = "Expected type: " + ty.get()->get_source_type() +
            " for global variable decl";
        diag->print_error(exp->sr, err.c_str());

        return false;
    }

    env[global_env][id] = ty.get();

    return true;
}

bool GlobalDecl::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {
    (void)ctxt;
    (void)out;
    (void)diag;

    return true;
}
