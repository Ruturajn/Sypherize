#include "../inc/ast.h"
#include <memory>

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

bool FunDecl::compile(LLCtxt& ctxt, LLOut& out, LLGout& gout,
                      Diagnostics* diag, LLProg& llprog) const {

    (void)gout;

    LLCtxt fun_ctxt = ctxt;

    std::unique_ptr<LLType> fun_ret_ty(this->frtype->compile_type());
    std::vector<LLType*> arg_ty_list;
    std::vector<std::string> arg_uid_list;

    for (auto& arg: this->args) {
        auto arg_uid = gentemp_ll(arg.second);

        // Alloca the argument
        auto alloca_uid = gentemp_ll("alloca_arg");
        std::unique_ptr<LLType> alloca_type_ptr(arg.first->compile_type());
        auto alloca_insn = new LLEInsn(alloca_uid,
            std::make_unique<LLIAlloca>(std::move(alloca_type_ptr), "")
        );
        out.second->stream.push_back(alloca_insn);

        std::unique_ptr<LLType> id_base_ty(arg.first->compile_type());
        auto id_ty = std::make_unique<LLTPtr>(std::move(id_base_ty));

        fun_ctxt[arg.second] = {id_ty.release(), new LLOId(alloca_uid)};

        // Store from the argument into the allocated space
        auto store_ty = fun_ctxt[arg.second].first->get_underlying_type()->clone();
        auto store_insn = std::make_unique<LLIStore>(
            std::move(store_ty),
            std::make_unique<LLOId>(arg_uid),
            std::make_unique<LLOId>(alloca_uid)
        );
        out.second->stream.push_back(new LLEInsn(gentemp_ll("store"), std::move(store_insn)));


        arg_ty_list.push_back(arg.first->compile_type());
        arg_uid_list.push_back(arg_uid);
    }

    std::unique_ptr<LLType> func_ty(new LLTFunc(arg_ty_list, std::move(fun_ret_ty)));

    fun_ctxt[this->fname] = {new LLTPtr(std::move(func_ty)), new LLOGid(this->fname)};

    auto func_ret_ty = fun_ctxt[this->fname].first->clone().release();

    for (auto s : block) {
        if (s->compile(fun_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }

    // Free up memory used by fun_ctxt
    for (auto& elem: fun_ctxt) {
        if (ctxt.find(elem.first) == ctxt.end()) {
            delete elem.second.first;
            delete elem.second.second;
        }
    }

    ctxt[this->fname] = {
        func_ret_ty,
        new LLOGid(this->fname)
    };


    /// CFG
    auto cfg = new LLCFG;
    out.second->create_cfg(*cfg);

    std::vector<LLType*> arg_type_list;

    for (auto& arg: this->args)
        arg_type_list.push_back(arg.first->compile_type());

    std::unique_ptr<LLType> fun_ret_ty_ptr(this->frtype->compile_type());

    std::unique_ptr<LLCFG> cfg_ptr(cfg);

    llprog.fdecls.push_back({
        this->fname,
        new LLFDecl(arg_type_list, std::move(fun_ret_ty_ptr), arg_uid_list,
                    std::move(cfg_ptr))
    });

    /* std::cout << "\n\n"; */
    /* out.second->print_ll_stream(std::cout); */
    /* std::cout << "\n\n"; */

    // Delete instruction stream for decl
    /* for (auto& elem: out.second->stream) */
    /*     delete elem; */

    out.second->stream.clear();

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

bool GlobalDecl::compile(LLCtxt& ctxt, LLOut& out, LLGout& gout,
                         Diagnostics* diag, LLProg& llprog) const {

    (void)out;
    return compile_global(ctxt, gout, diag, llprog);

}

bool GlobalDecl::compile_global(LLCtxt& ctxt, LLGout& gout,
                                Diagnostics* diag, LLProg& llprog) const {

    auto id_op = gentemp_ll(this->id);

    std::unique_ptr<LLType> id_base_ty(this->ty->compile_type());
    auto id_ty = std::make_unique<LLTPtr>(std::move(id_base_ty));

    ctxt[this->id] = {id_ty.release(), new LLOGid(id_op)};

    if (this->exp->compile_global(ctxt, gout, diag) == false) {
        diag->print_error(this->exp->sr, "[ICE] Unable to compile global decl");
        return false;
    }

    llprog.gdecls.push_back({id_op, gout.first});

    for (auto& elem: gout.second)
        llprog.gdecls.push_back(elem);

    gout.second.clear();

    return true;
}
