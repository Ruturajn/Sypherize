#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// NumberExpNode
///===-------------------------------------------------------------------===///

void NumberExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "NUM: " << val << "\n";
}

Type* NumberExpNode::typecheck(Environment& env,
                               const std::string& fname,
                               FuncEnvironment& fenv) const {
    (void)env;
    (void)fname;
    (void)fenv;
    return new TInt;
}

///===-------------------------------------------------------------------===///
/// StringExpNode
///===-------------------------------------------------------------------===///

void StringExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "STRING: " << val << "\n";
}

Type* StringExpNode::typecheck(Environment& env,
                               const std::string& fname,
                               FuncEnvironment& fenv) const {
    (void)env;
    (void)fname;
    (void)fenv;
    return new TString;
}

///===-------------------------------------------------------------------===///
/// BoolExpNode
///===-------------------------------------------------------------------===///

void BoolExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "BOOL: " << val << "\n";
}

Type* BoolExpNode::typecheck(Environment& env,
                             const std::string& fname,
                             FuncEnvironment& fenv) const {
    (void)env;
    (void)fname;
    (void)fenv;
    return new TBool;
}

///===-------------------------------------------------------------------===///
/// IdExpNode
///===-------------------------------------------------------------------===///

void IdExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "ID: " << val << "\n";
}

Type* IdExpNode::typecheck(Environment& env,
                           const std::string& fname,
                           FuncEnvironment& fenv) const {
    (void)fenv;
    if (env.find(fname) == env.end())
        return nullptr;

    if (env[fname].find(val) == env[fname].end()) {
        if (env["__global__"].find(val) == env["__global__"].end())
            return nullptr;

        return env["__global__"][val];
    }

    return env[fname][val];
}

///===-------------------------------------------------------------------===///
/// CArrExpNode
///===-------------------------------------------------------------------===///

void CArrExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "CARR:\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "TYPE:";
    ty->print_type();
    std::cout << "\n";

    for (auto &e: exp_list)
        e->print_node(indent + 4);
}

Type* CArrExpNode::typecheck(Environment& env,
                             const std::string& fname,
                             FuncEnvironment& fenv) const {
    Type* exp_ty = nullptr;

    for (auto& exp: exp_list) {
        exp_ty = exp->typecheck(env, fname, fenv);
        if (exp_ty == nullptr)
            return nullptr;

        if (!((*(ty.get())) == (*(exp_ty))))
            return nullptr;
    }

    return ty.get();
}

///===-------------------------------------------------------------------===///
/// NewExpNode
///===-------------------------------------------------------------------===///

void NewExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "NEWEXP:\n";
    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "TYPE:";
    ty->print_type();
    std::cout << "\n";

    if (exp != nullptr)
        exp->print_node(indent + 4);
    else {
        for (int i = 0; i < indent + 4; i++)
            std::cout << " ";
        std::cout << "SIZE: 1\n";
    }
}

Type* NewExpNode::typecheck(Environment& env,
                            const std::string& fname,
                            FuncEnvironment& fenv) const {
    if (exp != nullptr) {
        if (exp->typecheck(env, fname, fenv) == nullptr)
            return nullptr;
    }

    return ty.get();
}

///===-------------------------------------------------------------------===///
/// IndexExpNode
///===-------------------------------------------------------------------===///

void IndexExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "INDEX:\n";

    exp->print_node(indent + 4);
    idx->print_node(indent + 4);
}

Type* IndexExpNode::typecheck(Environment& env,
                              const std::string& fname,
                              FuncEnvironment& fenv) const {

    // If exp is of type TArray or TRef, it is indexable.
    auto exp_type = exp->typecheck(env, fname, fenv);
    if (exp_type == nullptr)
        return nullptr;

    if (!(*exp_type).is_indexable)
        return nullptr;

    // If idx is of type TInt, this node is typechecked.
    auto idx_type = idx->typecheck(env, fname, fenv);
    if (idx_type == nullptr)
        return nullptr;

    if (!(*idx_type).is_index)
        return nullptr;

    return exp_type->get_underlying_type();
}

///===-------------------------------------------------------------------===///
/// BinopExpNode
///===-------------------------------------------------------------------===///

void BinopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "BINOP: ";

    switch (binop) {
        case BinopType::BINOP_PLUS:
            std::cout << "+";
            break;
        case BinopType::BINOP_MINUS:
            std::cout << "-";
            break;
        case BinopType::BINOP_MULT:
            std::cout << "*";
            break;
        case BinopType::BINOP_DIVIDE:
            std::cout << "/";
            break;
        case BinopType::BINOP_MODULUS:
            std::cout << "%";
            break;
        case BinopType::BINOP_LSHIFT:
            std::cout << "<<";
            break;
        case BinopType::BINOP_RSHIFT:
            std::cout << ">>";
            break;
        case BinopType::BINOP_BITAND:
            std::cout << "&";
            break;
        case BinopType::BINOP_BITOR:
            std::cout << "|";
            break;
        case BinopType::BINOP_BITXOR:
            std::cout << "^";
            break;
        case BinopType::BINOP_LT:
            std::cout << "<";
            break;
        case BinopType::BINOP_LTE:
            std::cout << "<=";
            break;
        case BinopType::BINOP_GT:
            std::cout << ">";
            break;
        case BinopType::BINOP_GTE:
            std::cout << ">=";
            break;
        case BinopType::BINOP_EQEQUAL:
            std::cout << "==";
            break;
        case BinopType::BINOP_NEQUAL:
            std::cout << "!=";
            break;
        case BinopType::BINOP_LOGAND:
            std::cout << "&&";
            break;
        case BinopType::BINOP_LOGOR:
            std::cout << "||";
            break;
    }
    std::cout << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "LHS:\n";
    left->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "RHS:\n";
    right->print_node(indent + 8);
}

Type* BinopExpNode::typecheck(Environment& env,
                              const std::string& fname,
                              FuncEnvironment& fenv) const {

    auto left_type = left->typecheck(env, fname, fenv);
    auto right_type = right->typecheck(env, fname, fenv);

    if (left_type == nullptr || right_type == nullptr)
        return nullptr;

    if (!((*left_type) == (*right_type)))
        return nullptr;

    if (!(left_type->is_valid_binop(binop) &&
                right_type->is_valid_binop(binop)))
        return nullptr;

    return left_type;
}

///===-------------------------------------------------------------------===///
/// UnopExpNode
///===-------------------------------------------------------------------===///

void UnopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "UNOP:";
    switch (uop) {
        case UnopType::UNOP_NEG:
            std::cout << "~";
            break;
        case UnopType::UNOP_NOT:
            std::cout << "!";
            break;
        case UnopType::UNOP_DEREF:
            std::cout << "@";
            break;
        case UnopType::UNOP_ADDROF:
            std::cout << "#";
            break;
    }
    std::cout << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "EXP:\n";
    exp->print_node(indent + 8);
}

Type* UnopExpNode::typecheck(Environment& env,
                             const std::string& fname,
                             FuncEnvironment& fenv) const {

    auto exp_type = exp->typecheck(env, fname, fenv);

    if (exp_type == nullptr ||
            (!exp_type->is_valid_unop(uop)))
        return nullptr;

    const NumberExpNode num(0, SRange(SLoc(0, 0), SLoc(0, 0)));
    const ExpNode& e = *(exp.get());

    switch (uop) {
        case UnopType::UNOP_NEG:
        case UnopType::UNOP_NOT:
            return exp_type;

        case UnopType::UNOP_DEREF: {
            if (typeid(e) == typeid(num))
                return nullptr;

            return exp_type->get_underlying_type();
        }

        case UnopType::UNOP_ADDROF:
            if (typeid(e) == typeid(num))
                return nullptr;

            return new TRef(exp_type);

        default:
            return nullptr;
    }
}

///===-------------------------------------------------------------------===///
/// FunCallExpNode
///===-------------------------------------------------------------------===///

void FunCallExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "FUNCALL:\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNNAME: " << func_name << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "FUNARGS:\n";

    for (auto &arg: func_args)
        arg->print_node(indent + 8);
}

Type* FunCallExpNode::typecheck(Environment& env,
                                const std::string& fname,
                                FuncEnvironment& fenv) const {
    if (fenv.find(func_name) == fenv.end())
        return nullptr;

    if ((fenv[func_name].size() - 1) != func_args.size())
        return nullptr;

    Type* ret_type = fenv[func_name][0];

    Type* arg_ty = nullptr;
    for (int i = 0; i < (int)func_args.size(); i++) {
        arg_ty = func_args[i]->typecheck(env, fname, fenv);

        if (arg_ty == nullptr)
            return nullptr;

        if (!((*arg_ty) == (*fenv[func_name][i + 1])))
            return nullptr;
    }

    return ret_type;
}
