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
