#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// AssnStmtNode
///===-------------------------------------------------------------------===///

void AssnStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "ASSN:\n";

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
                             FuncEnvironment& fenv) const {

    auto lhs_type = lhs->typecheck(env, fname, fenv);
    auto rhs_type = rhs->typecheck(env, fname, fenv);

    if (lhs_type == nullptr || rhs_type == nullptr)
        return false;

    return (*lhs_type) == (*rhs_type);
}

///===-------------------------------------------------------------------===///
/// DeclStmtNode
///===-------------------------------------------------------------------===///

void DeclStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "DECL:\n";

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
                             FuncEnvironment& fenv) const {

    auto exp_ty = exp->typecheck(env, fname, fenv);

    if (exp_ty == nullptr)
        return false;

    Type* base_ty = ty.get();

    if (exp->is_indirect)
        base_ty = base_ty->get_underlying_type();

    if (base_ty == nullptr)
        return false;

    if ((*exp_ty) != (*base_ty))
        return false;

    // TODO: Print out good error messages, this one is a huge problem
    if (env.find(fname) == env.end())
        return false;

    if (env[fname].find(id) != env[fname].end())
        return false;

    env[fname][id] = ty.get();

    return true;
}

///===-------------------------------------------------------------------===///
/// SCallStmtNode
///===-------------------------------------------------------------------===///

void SCallStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "SFUNCALL:\n";

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
                             FuncEnvironment& fenv) const {

    if (fenv.find(this->fname) == fenv.end())
        return false;

    if ((fenv[this->fname].size() - 1) != fargs.size())
        return false;

    for (int i = 0; i < (int)fargs.size(); i++) {
        auto arg_ty = fargs[i]->typecheck(env, fname, fenv);

        if (arg_ty == nullptr)
            return false;

        if (!((*arg_ty) == (*fenv[this->fname][i + 1])))
            return false;
    }

    return true;
}

///===-------------------------------------------------------------------===///
/// RetStmtNode
///===-------------------------------------------------------------------===///

void RetStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "RETURN:\n";

    if (exp != nullptr)
        exp->print_node(indent + 4);
    else {
        for (int i = 0; i < indent + 4; i++)
            std::cout << " ";

        std::cout << "void\n";
    }
}

bool RetStmtNode::typecheck(Environment& env, const std::string& fname,
                             FuncEnvironment& fenv) const {

    if (fenv.find(fname) == fenv.end())
        return false;

    auto fret = fenv[fname][0];

    if (exp == nullptr)
        return fret->is_void;

    auto exp_ty = exp->typecheck(env, fname, fenv);

    if (exp_ty == nullptr)
        return false;

    return (*exp_ty) == (*fret);
}

///===-------------------------------------------------------------------===///
/// IfStmtNode
///===-------------------------------------------------------------------===///

void IfStmtNode::print_stmt(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "IF:\n";

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
                             FuncEnvironment& fenv) const {

    if (cond->typecheck(env, fname, fenv) == nullptr)
        return false;

    Environment env_if = env;

    for (auto& s: then_body) {
        if (s->typecheck(env_if, fname, fenv) == false)
            return false;
    }

    Environment env_else = env;

    for (auto& s: else_body) {
        if (s->typecheck(env_else, fname, fenv) == false)
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

    std::cout << "FOR:\n";

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
                             FuncEnvironment& fenv) const {

    Environment env_decl = env;

    for (auto& d: decl_list) {
        if (d->typecheck(env_decl, fname, fenv) == false)
            return false;
    }

    if (cond->typecheck(env_decl, fname, fenv) == nullptr)
        return false;

    if (iter->typecheck(env_decl, fname, fenv) == false)
        return false;

    for (auto& s: body) {
        if (s->typecheck(env_decl, fname, fenv) == false)
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

    std::cout << "WHILE:\n";

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
                             FuncEnvironment& fenv) const {

    if (cond->typecheck(env, fname, fenv) == nullptr)
        return false;

    Environment env_body = env;

    for (auto& s: body) {
        if (s->typecheck(env_body, fname, fenv) == false)
            return false;
    }

    return true;
}
