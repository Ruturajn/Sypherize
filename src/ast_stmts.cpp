#include "../inc/ast.h"

using namespace AST;

///===-------------------------------------------------------------------===///
/// AssnStmtNode
///===-------------------------------------------------------------------===///

void AssnStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "ASSN";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "LHS:\n";
    lhs->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "RHS:\n";
    rhs->print_node(indent + 8);
}

bool AssnStmtNode::typecheck(Environment& env, const std::string& fname,
                             FuncEnvironment& fenv, Diagnostics* diag) const {

    auto lhs_type = lhs->typecheck(env, fname, fenv, diag);
    auto rhs_type = rhs->typecheck(env, fname, fenv, diag);

    if (lhs_type == nullptr || rhs_type == nullptr) {
        if (lhs_type == nullptr)
            diag->print_error(lhs->sr, "Invalid type for LHS of assignment");
        if (rhs_type == nullptr)
            diag->print_error(rhs->sr, "Invalid type for RHS of assignment");
        return false;
    }

    if ((*lhs_type) != (*rhs_type)) {
        std::string err = "Expected type: " + lhs_type->get_source_type() +
            " for assignment RHS";
        diag->print_error(rhs->sr, err.c_str());
        return false;
    }

    return true;
}

bool AssnStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    if (this->lhs->compile(ctxt, out, diag, true) == false) {
        diag->print_error(this->lhs->sr, "[ICE] Unable to compile expression"
                            " as LHS during AssnStmtNode");
        return false;
    }

    auto lhs_op = out.first.second->clone();

    if (this->rhs->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->rhs->sr, "[ICE] Unable to compile expression"
                            " during AssnStmtNode");
        return false;
    }

    auto rhs_ty = out.first.first->clone();
    auto rhs_op = out.first.second->clone();

    // Emit store
    auto store_ty = std::move(rhs_ty);
    auto store_insn = std::make_unique<LLIStore>(
        std::move(store_ty),
        std::move(rhs_op),
        std::move(lhs_op)
    );

    auto res_uid = gentemp_ll("store");

    out.second->stream.push_back(new LLEInsn(res_uid, std::move(store_insn)));

    return true;
}

///===-------------------------------------------------------------------===///
/// DeclStmtNode
///===-------------------------------------------------------------------===///

void DeclStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "DECL";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "TYPE: ";
    if (ty != nullptr)
        ty->print_type();
    else
        std::cout << "Array";
    std::cout << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "ID: " << id << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "EXP:\n";
    exp->print_node(indent + 8);
}

bool DeclStmtNode::typecheck(Environment& env, const std::string& fname,
                             FuncEnvironment& fenv, Diagnostics* diag) const {

    const TVoid tv;

    Type* base_ty = ty.get();

    if (tv == *base_ty) {
        diag->print_error(sr, "Invalid `void` decl type");
        return false;
    }

    auto exp_ty = exp->typecheck(env, fname, fenv, diag);

    if (exp_ty == nullptr)
        return false;


    if (exp->is_indirect) {
        const TRef t(nullptr);
        if (typeid(*exp_ty) != typeid(t)) {
            diag->print_error(sr, "Expected reference type for decl type");
            return false;
        }

        base_ty = base_ty->get_underlying_type();
    }

    if (base_ty == nullptr) {
        if (exp->is_indirect)
            diag->print_error(sr, "Expected decl type to be an indirect type");
        else
            diag->print_error(sr, "Invalid variable decl type");
        return false;
    }

    if ((*exp_ty) != (*base_ty)) {
        std::string err = "Expected type: " + base_ty->get_source_type() +
            " for variable decl";
        diag->print_error(exp->sr, err.c_str());
        return false;
    }

    if (env.find(fname) == env.end()) {
        diag->print_error(sr, "[ICE] Invalid `fname` passed to DeclStmtNode");
        return false;
    }

    if (env[fname].find(id) != env[fname].end()) {
        diag->print_error(sr, "Redefinition of variable");
        return false;
    }

    env[fname][id] = ty.get();

    return true;
}

bool DeclStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    std::unique_ptr<LLType> id_base_ty(this->ty->compile_type());
    auto id_ty = std::make_unique<LLTPtr>(std::move(id_base_ty));
    ctxt[this->id].first = id_ty.release();

    auto alloca_ty = ctxt[this->id].first->get_underlying_type()->clone();
    auto alloca_insn = std::make_unique<LLIAlloca>(std::move(alloca_ty), "");

    auto alloca_uid = gentemp_ll(this->id);

    ctxt[this->id].second = new LLOId(alloca_uid);
    out.second->stream.push_back(new LLEInsn(alloca_uid, std::move(alloca_insn)));

    if (this->exp->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->exp->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto store_ty = out.first.first->clone();
    auto store_op = out.first.second->clone();

    auto store_insn = std::make_unique<LLIStore>(
        std::move(store_ty),
        std::move(store_op),
        std::make_unique<LLOId>(alloca_uid)
    );

    auto res_uid = gentemp_ll("store");


    out.second->stream.push_back(new LLEInsn(res_uid, std::move(store_insn)));

    return true;
}

///===-------------------------------------------------------------------===///
/// SCallStmtNode
///===-------------------------------------------------------------------===///

void SCallStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "SFUNCALL";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNNAME: " << fname << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNARGS:\n";

    for (auto &arg: fargs)
        arg->print_node(indent + 8);
}

bool SCallStmtNode::typecheck(Environment& env, const std::string& fname,
                              FuncEnvironment& fenv, Diagnostics* diag) const {

    if (fenv.find(this->fname) == fenv.end()) {
        diag->print_error(sr, "[ICE] Invalid `fname` passed to SCallStmtNode");
        return false;
    }

    if ((fenv[this->fname].size() - 1) != fargs.size()) {
        if ((fenv[this->fname].size() - 1) > fargs.size())
            diag->print_error(sr, "Too few arguments for function call");
        else
            diag->print_error(sr, "Too many arguments for function call");
        return false;
    }

    for (int i = 0; i < (int)fargs.size(); i++) {
        auto arg_ty = fargs[i]->typecheck(env, fname, fenv, diag);

        if (arg_ty == nullptr) {
            diag->print_error(sr, "Invalid function argument type");
            return false;
        }

        if (!((*arg_ty) == (*fenv[this->fname][i + 1]))) {
            std::string err = "Expected type: " + fenv[this->fname][i+1]->get_source_type() +
                "for function argument";
            diag->print_error(fargs[i]->sr, err.c_str());
            return false;
        }
    }

    return true;
}

bool SCallStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    std::vector<std::pair<LLType*, LLOperand*>> ty_arg_list {};

    for (auto arg: this->fargs) {
        if (arg->compile(ctxt, out, diag, false) == false) {
            diag->print_error(arg->sr, "[ICE] Unable to compile expression");
            return false;
        }
        ty_arg_list.push_back({out.first.first->clone().release(),
                out.first.second->clone().release()});
    }

    if (ctxt.find(this->fname) ==  ctxt.end()) {
        diag->print_error(sr, "[ICE] Call to unknown function during compilation");
        return false;
    }

    if (ctxt[this->fname].first->get_underlying_type() == nullptr) {
        diag->print_error(sr, "[ICE] Expected pointer type during"
                            " compiling a FunCallExpNode");
        return false;
    }

    auto call_ty_raw = ctxt[this->fname].first->get_underlying_type()->get_return_type();

    if (call_ty_raw == nullptr) {
        diag->print_error(sr, "[ICE] Expected pointer to a function type during"
                            " compiling a FunCallExpNode");
        return false;
    }

    auto call_ty = call_ty_raw->clone();

    auto call_uid = gentemp_ll("funcall");
    auto call_insn = std::make_unique<LLICall>(
        std::move(call_ty),
        std::make_unique<LLOGid>(this->fname),
        ty_arg_list
    );

    return true;
}

///===-------------------------------------------------------------------===///
/// RetStmtNode
///===-------------------------------------------------------------------===///

void RetStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "RETURN";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    if (exp != nullptr)
        exp->print_node(indent + 4);
    else {
        for (int i = 0; i < indent + 4; i++)
            std::cout << " ";

        std::cout << "void\n";
    }
}

bool RetStmtNode::typecheck(Environment& env, const std::string& fname,
                            FuncEnvironment& fenv, Diagnostics* diag) const {

    if (fenv.find(fname) == fenv.end()) {
        diag->print_error(sr, "[ICE] Invalid `fname` passed to RetStmtNode");
        return false;
    }

    auto fret = fenv[fname][0];

    if (exp == nullptr) {
        if (!fret->is_void) {
            diag->print_error(sr, "Expected `void` return type");
            return false;
        }
        return true;
    }

    auto exp_ty = exp->typecheck(env, fname, fenv, diag);

    if (exp_ty == nullptr) {
        diag->print_error(exp->sr, "Invalid type for return expression");
        return false;
    }

    if ((*exp_ty) != (*fret)) {
        std::string err = "Expected type: " + fret->get_source_type() +
            "for function return value";
        diag->print_error(exp->sr, err.c_str());
        return false;
    }

    return true;
}

bool RetStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    if (exp == nullptr) {
        auto ret_ty = std::make_unique<LLTVoid>();
        out.second->stream.push_back(new LLETerm(
            std::make_unique<LLTermRet>(std::move(ret_ty), nullptr))
        );
        return true;
    }

    if (this->exp->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->exp->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto ret_op = out.first.second->clone();
    auto ret_ty = out.first.first->clone();

    out.second->stream.push_back(new LLETerm(
        std::make_unique<LLTermRet>(std::move(ret_ty), std::move(ret_op)))
    );

    return true;
}

///===-------------------------------------------------------------------===///
/// IfStmtNode
///===-------------------------------------------------------------------===///

void IfStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "IF";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "COND:\n";
    cond->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "THEN_BODY:\n";
    for (auto &s: then_body)
        s->print_stmt(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "ELSE_BODY:\n";
    for (auto &s: else_body)
        s->print_stmt(indent + 8);
}

bool IfStmtNode::typecheck(Environment& env, const std::string& fname,
                           FuncEnvironment& fenv, Diagnostics* diag) const {

    if (cond->typecheck(env, fname, fenv, diag) == nullptr)
        return false;

    Environment env_if = env;

    for (auto& s: then_body) {
        if (s->typecheck(env_if, fname, fenv, diag) == false)
            return false;
    }

    Environment env_else = env;

    for (auto& s: else_body) {
        if (s->typecheck(env_else, fname, fenv, diag) == false)
            return false;
    }

    return true;
}

bool IfStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    // If Condition
    LLCtxt if_ctxt = ctxt;

    if (this->cond->compile(if_ctxt, out, diag, false) == false) {
        diag->print_error(this->cond->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto cond_ty = out.first.first->clone();
    auto cond_op = out.first.second->clone();

    auto true_lbl = gentemp_ll("if_then");
    auto else_lbl = gentemp_ll("if_else");
    auto merge_lbl = gentemp_ll("if_merge");

    auto cbr_term = std::make_unique<LLTermCbr>(
        std::move(cond_op),
        true_lbl,
        else_lbl
    );
    out.second->stream.push_back(new LLETerm(std::move(cbr_term)));

    // If Then body
    out.second->stream.push_back(new LLELables(true_lbl));
    for (auto s : this->then_body) {
        if (s->compile(if_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }
    auto br_term = std::make_unique<LLTermBr>(merge_lbl);
    out.second->stream.push_back(new LLETerm(std::move(br_term)));

    // Else body
    out.second->stream.push_back(new LLELables(else_lbl));
    LLCtxt else_ctxt = ctxt;
    for (auto s : this->else_body) {
        if (s->compile(else_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }
    auto else_br_term = std::make_unique<LLTermBr>(merge_lbl);
    out.second->stream.push_back(new LLETerm(std::move(else_br_term)));

    // Merge Lbl
    out.second->stream.push_back(new LLELables(merge_lbl));

    return true;
}

///===-------------------------------------------------------------------===///
/// ForStmtNode
///===-------------------------------------------------------------------===///

void ForStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "FOR";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "DECL_LIST:\n";
    for (auto &d: decl_list)
        d->print_stmt(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "COND:\n";
    cond->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "ITER:\n";
    iter->print_stmt(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "BODY:\n";
    for (auto &b: body)
        b->print_stmt(indent + 8);
}

bool ForStmtNode::typecheck(Environment& env, const std::string& fname,
                            FuncEnvironment& fenv, Diagnostics* diag) const {

    Environment env_decl = env;

    for (auto& d: decl_list) {
        if (d->typecheck(env_decl, fname, fenv, diag) == false)
            return false;
    }

    if (cond->typecheck(env_decl, fname, fenv, diag) == nullptr)
        return false;

    if (iter->typecheck(env_decl, fname, fenv, diag) == false)
        return false;

    for (auto& s: body) {
        if (s->typecheck(env_decl, fname, fenv, diag) == false)
            return false;
    }

    return true;
}

bool ForStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    // For decls
    LLCtxt for_ctxt = ctxt;
    for (auto s : decl_list) {
        if (s->compile(for_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }
    auto for_cond_lbl = gentemp_ll("for_cond");
    out.second->stream.push_back(
        new LLETerm(std::make_unique<LLTermBr>(for_cond_lbl))
    );

    // For Condition
    out.second->stream.push_back(new LLELables(for_cond_lbl));
    if (this->cond->compile(for_ctxt, out, diag, false) == false) {
        diag->print_error(this->cond->sr, "[ICE] Unable to compile expression");
        return false;
    }
    auto cond_op = out.first.second->clone();

    auto for_body_lbl = gentemp_ll("for_body");
    auto for_exit_lbl = gentemp_ll("for_exit");

    auto cbr_term = std::make_unique<LLTermCbr>(
        std::move(cond_op),
        for_body_lbl,
        for_exit_lbl
    );
    out.second->stream.push_back(new LLETerm(std::move(cbr_term)));

    // For Body
    out.second->stream.push_back(new LLELables(for_body_lbl));
    for (auto s : this->body) {
        if (s->compile(for_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }

    if (iter->compile(for_ctxt, out, diag) == false) {
        diag->print_error(iter->sr, "[ICE] Unable to compile stmt");
        return false;
    }

    auto br_term = std::make_unique<LLTermBr>(for_cond_lbl);
    out.second->stream.push_back(new LLETerm(std::move(br_term)));

    // For Exit
    out.second->stream.push_back(new LLELables(for_exit_lbl));

    return true;
}

///===-------------------------------------------------------------------===///
/// WhileStmtNode
///===-------------------------------------------------------------------===///

void WhileStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "WHILE";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "COND:\n";
    cond->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "BODY:\n";
    for (auto &b: body)
        b->print_stmt(indent + 8);
}

bool WhileStmtNode::typecheck(Environment& env, const std::string& fname,
                              FuncEnvironment& fenv, Diagnostics* diag) const {

    if (cond->typecheck(env, fname, fenv, diag) == nullptr)
        return false;

    Environment env_body = env;

    for (auto& s: body) {
        if (s->typecheck(env_body, fname, fenv, diag) == false)
            return false;
    }

    return true;
}

bool WhileStmtNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag) const {

    LLCtxt while_ctxt = ctxt;

    auto while_cond_lbl = gentemp_ll("while_cond");
    out.second->stream.push_back(
        new LLETerm(std::make_unique<LLTermBr>(while_cond_lbl))
    );

    // While Condition
    out.second->stream.push_back(new LLELables(while_cond_lbl));
    if (this->cond->compile(while_ctxt, out, diag, false) == false) {
        diag->print_error(this->cond->sr, "[ICE] Unable to compile expression");
        return false;
    }
    auto cond_op = out.first.second->clone();

    auto while_body_lbl = gentemp_ll("while_body");
    auto while_exit_lbl = gentemp_ll("while_exit");

    auto cbr_term = std::make_unique<LLTermCbr>(
        std::move(cond_op),
        while_body_lbl,
        while_exit_lbl
    );
    out.second->stream.push_back(new LLETerm(std::move(cbr_term)));

    // While Body
    out.second->stream.push_back(new LLELables(while_body_lbl));
    for (auto s : this->body) {
        if (s->compile(while_ctxt, out, diag) == false) {
            diag->print_error(s->sr, "[ICE] Unable to compile stmt");
            return false;
        }
    }

    auto br_term = std::make_unique<LLTermBr>(while_cond_lbl);
    out.second->stream.push_back(new LLETerm(std::move(br_term)));

    // While Exit
    out.second->stream.push_back(new LLELables(while_exit_lbl));
    return true;
}
