#include "../inc/ast.h"
#include <memory>

using namespace AST;

///===-------------------------------------------------------------------===///
/// NumberExpNode
///===-------------------------------------------------------------------===///

void NumberExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "NUM";
    sr.print_srange(std::cout);
    std::cout << ": " << val << "\n";
}

Type* NumberExpNode::typecheck(Environment& env,
                               const std::string& fname,
                               FuncEnvironment& fenv,
                               Diagnostics* diag) const {
    (void)fname;
    (void)fenv;
    (void)diag;
    return env["__global__"]["int"];
}

bool NumberExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                            bool is_lhs) const {

    (void)ctxt;

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile NumberExpNode as LHS");
        return false;
    }

    out.first.first = ctxt["int"].first;

    auto num_op = std::make_unique<LLOId>(gentemp_ll("num_exp"));
    out.first.second = std::move(num_op);
    return true;
}

///===-------------------------------------------------------------------===///
/// StringExpNode
///===-------------------------------------------------------------------===///

void StringExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "STRING";
    sr.print_srange(std::cout);
    std::cout << ": " << val << "\n";
}

Type* StringExpNode::typecheck(Environment& env,
                               const std::string& fname,
                               FuncEnvironment& fenv,
                               Diagnostics* diag) const {
    (void)fname;
    (void)fenv;
    (void)diag;
    return env["__global__"]["string"];
}

bool StringExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                            bool is_lhs) const {

    (void)ctxt;

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile StringExpNode as LHS");
        return false;
    }

    out.first.first = ctxt["string"].first;

    auto op_uid = gentemp_ll("gep_str_op");
    auto op_ptr = new LLOId(op_uid);
    std::unique_ptr<LLOId> str_op(op_ptr);
    out.first.second = std::move(str_op);

    auto string_gid = gentemp_ll("string_exp");

    auto gdecl = std::make_unique<LLGDecl>(
        std::make_unique<LLTArray>(this->val.size() + 1,
                                   std::make_unique<LLTi8>()),
        std::make_unique<LLGString>(this->val)
    );

    out.second->stream.push_back(new LLEGlbl(string_gid, std::move(gdecl)));

    auto str_arr_ty = std::make_unique<LLTArray>(this->val.size() + 1,
        std::make_unique<LLTi8>());

    auto gep_ty = std::make_unique<LLTPtr>(std::move(str_arr_ty));
    auto gep_insn = new LLIGep(
        std::move(gep_ty),
        std::make_unique<LLOId>(string_gid),
        {new LLOConst(0), new LLOConst(0)}
    );

    std::unique_ptr<LLIGep> gep_insn_ptr(gep_insn);
    out.second->stream.push_back(new LLEInsn(op_uid, std::move(gep_insn_ptr)));

    return true;
}

///===-------------------------------------------------------------------===///
/// BoolExpNode
///===-------------------------------------------------------------------===///

void BoolExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "BOOL";
    sr.print_srange(std::cout);
    std::cout << ": " << val << "\n";
}

Type* BoolExpNode::typecheck(Environment& env,
                             const std::string& fname,
                             FuncEnvironment& fenv,
                             Diagnostics* diag) const {
    (void)fname;
    (void)fenv;
    (void)diag;
    return env["__global__"]["bool"];
}

bool BoolExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                          bool is_lhs) const{
    (void)ctxt;

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile BoolExpNode as LHS");
        return false;
    }

    out.first.first = ctxt["bool"].first;

    LLOConst* bool_op = nullptr;
    if (this->val)
        bool_op = new LLOConst(1);
    else
        bool_op = new LLOConst(0);

    std::unique_ptr<LLOConst> bool_op_ptr(bool_op);
    out.first.second = std::move(bool_op_ptr);

    return true;
}

///===-------------------------------------------------------------------===///
/// NullExpNode
///===-------------------------------------------------------------------===///

void NullExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "NULL:";
    sr.print_srange(std::cout);
    std::cout << "\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "TYPE:";
    ty->print_type();
    std::cout << "\n";
}

Type* NullExpNode::typecheck(Environment& env,
                             const std::string& fname,
                             FuncEnvironment& fenv,
                             Diagnostics* diag) const {
    (void)env;
    (void)fname;
    (void)fenv;
    (void)diag;

    const TRef tr(nullptr);
    auto null_ty = ty.get();

    if (typeid(tr) != typeid(*null_ty)) {
        diag->print_error(sr, "Non-ref type for `null` expression");
        return nullptr;
    }

    return ty.get();
}

bool NullExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                          bool is_lhs) const{
    (void)ctxt;

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile NullExpNode as LHS");
        return false;
    }

    auto null_ty = this->ty->compile_type();
    out.first.first = null_ty;
    out.first.second = nullptr;

    return true;
}

///===-------------------------------------------------------------------===///
/// IdExpNode
///===-------------------------------------------------------------------===///

void IdExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "ID";
    sr.print_srange(std::cout);
    std::cout << ": " << val << "\n";
}

Type* IdExpNode::typecheck(Environment& env,
                           const std::string& fname,
                           FuncEnvironment& fenv,
                           Diagnostics* diag) const {
    (void)fenv;

    if (env.find(fname) == env.end()) {
        diag->print_error(sr, "[ICE] Invalid `fname` passed to IdExpNode");
        return nullptr;
    }

    if (env[fname].find(val) == env[fname].end()) {
        if (env["__global__"].find(val) == env["__global__"].end()) {
            diag->print_error(sr, "Unknown variable accessed");
            return nullptr;
        }

        return env["__global__"][val];
    }

    return env[fname][val];
}

bool IdExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                        bool is_lhs) const{
    (void)ctxt;

    if (ctxt.find(this->val) == ctxt.end()) {
        diag->print_error(sr, "[ICE] Encountered unknown identifier "
                "during compilation");
        return false;
    }

    out.first.first = ctxt[this->val].first;

    auto id_op_uid = gentemp_ll(this->val);
    out.first.second = std::make_unique<LLOId>(id_op_uid);

    auto res_uid = gentemp_ll("id_exp");

    std::unique_ptr<LLType> load_ty(new LLTPtr(ctxt[this->val].first->clone()));
    auto load_insn = std::make_unique<LLILoad>(std::move(load_ty),
        std::make_unique<LLOId>(id_op_uid));

    out.second->stream.push_back(new LLEInsn(res_uid, std::move(load_insn)));

    return true;
}

///===-------------------------------------------------------------------===///
/// CArrExpNode
///===-------------------------------------------------------------------===///

void CArrExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "CARR";
    sr.print_srange(std::cout);
    std::cout << ":\n";

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
                             FuncEnvironment& fenv,
                             Diagnostics* diag) const {
    Type* exp_ty = nullptr;

    Type* base_ty = ty.get()->get_underlying_type();

    if (base_ty == nullptr) {
        diag->print_error(sr, "Expected array type for array initializer"
                " expression");
        return nullptr;
    }

    for (auto& exp: exp_list) {
        exp_ty = exp->typecheck(env, fname, fenv, diag);

        if (exp_ty == nullptr || (*(base_ty) != (*exp_ty))) {
            std::string err = "Expected type: " + base_ty->get_source_type() +
                " for array initialization";
            diag->print_error(exp->sr, err.c_str());
            return nullptr;
        }
    }

    return ty.get();
}

bool CArrExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                          bool is_lhs) const{
    (void)ctxt;


    return true;
}

///===-------------------------------------------------------------------===///
/// NewExpNode
///===-------------------------------------------------------------------===///

void NewExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "NEWEXP";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "TYPE:";
    ty->print_type();
    std::cout << "\n";

    if (exp_list.size() != 0) {
        for (auto& x : exp_list)
            x->print_node(indent + 4);
    }
    else {
        for (int i = 0; i < indent + 4; i++)
            std::cout << " ";
        std::cout << "SIZE: 1\n";
    }
}

Type* NewExpNode::typecheck(Environment& env,
                            const std::string& fname,
                            FuncEnvironment& fenv,
                            Diagnostics* diag) const {

    if (exp_list.size() != 0) {
            for (auto&x : exp_list) {
            auto exp_ty = x->typecheck(env, fname, fenv, diag);
            if (exp_ty == nullptr || exp_ty->is_index == false) {
                diag->print_error(x->sr, "Expected `int` type for the size"
                        " expression");
                return nullptr;
            }
        }
    }

    return ty.get();
}

///===-------------------------------------------------------------------===///
/// IndexExpNode
///===-------------------------------------------------------------------===///

void IndexExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "INDEX";
    sr.print_srange(std::cout);
    std::cout << ":\n";

    exp->print_node(indent + 4);
    for (auto &e: idx_list)
        e->print_node(indent + 4);
}

Type* IndexExpNode::typecheck(Environment& env,
                              const std::string& fname,
                              FuncEnvironment& fenv,
                              Diagnostics* diag) const {

    // If exp is of type TArray or TRef, it is indexable.
    auto exp_type = exp->typecheck(env, fname, fenv, diag);
    if (exp_type == nullptr || !(exp_type->is_indexable)) {
        diag->print_error(exp->sr, "Expected indexable type for index operation");
        return nullptr;
    }

    // If idx is of type TInt, this node is typechecked.
    for (auto& idx: idx_list) {
        auto idx_type = idx->typecheck(env, fname, fenv, diag);
        if (idx_type == nullptr || !(idx_type->is_index)) {
            diag->print_error(idx->sr, "Expected `int` type for the index"
                    " expression");
            return nullptr;
        }
    }

    return exp_type->get_underlying_type();
}

///===-------------------------------------------------------------------===///
/// BinopExpNode
///===-------------------------------------------------------------------===///

void BinopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "BINOP";
    sr.print_srange(std::cout);
    std::cout << ": ";

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
                              FuncEnvironment& fenv,
                              Diagnostics* diag) const {

    auto left_type = left->typecheck(env, fname, fenv, diag);
    auto right_type = right->typecheck(env, fname, fenv, diag);

    if (left_type == nullptr || right_type == nullptr) {
        if (left_type == nullptr)
            diag->print_error(left->sr, "Expected valid LHS expression for binop");
        if (right_type == nullptr)
            diag->print_error(right->sr, "Expected valid RHS expression for binop");
        return nullptr;
    }

    if (!((*left_type) == (*right_type))) {
        diag->print_error(sr, "Expected compatible types for LHS and RHS of"
                " binop expression");
        return nullptr;
    }

    if (!(left_type->is_valid_binop(binop) &&
                right_type->is_valid_binop(binop))) {
        if (!left_type->is_valid_binop(binop))
            diag->print_error(left->sr, "Expected valid type for LHS of"
                    " binop expression");
        if (!right_type->is_valid_binop(binop))
            diag->print_error(right->sr, "Expected valid type for RHS of"
                    " binop expression");
        return nullptr;
    }

    return left_type;
}

///===-------------------------------------------------------------------===///
/// UnopExpNode
///===-------------------------------------------------------------------===///

void UnopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "UNOP";
    sr.print_srange(std::cout);
    std::cout << ":";

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
                             FuncEnvironment& fenv,
                             Diagnostics* diag) const {

    auto exp_type = exp->typecheck(env, fname, fenv, diag);

    if (exp_type == nullptr ||
            (!exp_type->is_valid_unop(uop))) {
        diag->print_error(exp->sr, "Expected compatible type for"
                " unop expression");
        return nullptr;
    }

    const NumberExpNode num(0, SRange(SLoc(0, 0), SLoc(0, 0)));
    const ExpNode& e = *(exp.get());

    switch (uop) {
        case UnopType::UNOP_NEG:
        case UnopType::UNOP_NOT:
            return exp_type;

        case UnopType::UNOP_DEREF: {
            if (typeid(e) == typeid(num)) {
                diag->print_error(exp->sr, "Disallowed literal value for"
                        " unop expression");
                return nullptr;
            }

            return exp_type->get_underlying_type();
        }

        case UnopType::UNOP_ADDROF:
            if (typeid(e) == typeid(num)) {
                diag->print_error(exp->sr, "Disallowed literal value for"
                        " unop expression");
                return nullptr;
            }

            return new TRef(exp_type);

        default:
            diag->print_error(exp->sr, "[ICE] Unreachable case - during "
                    "typechecking UnopExpNode");
            return nullptr;
    }
}

///===-------------------------------------------------------------------===///
/// FunCallExpNode
///===-------------------------------------------------------------------===///

void FunCallExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "FUNCALL";
    sr.print_srange(std::cout);
    std::cout << ":\n";

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
                                FuncEnvironment& fenv,
                                Diagnostics* diag) const {

    if (fenv.find(func_name) == fenv.end()) {
        diag->print_error(sr, "Call to unknown function");
        return nullptr;
    }

    if ((fenv[func_name].size() - 1) != func_args.size()) {
        if ((fenv[func_name].size() - 1) > func_args.size())
            diag->print_error(sr, "Too few arguments for function call");
        else
            diag->print_error(sr, "Too many arguments for function call");

        return nullptr;
    }

    Type* ret_type = fenv[func_name][0];

    Type* arg_ty = nullptr;
    for (int i = 0; i < (int)func_args.size(); i++) {
        arg_ty = func_args[i]->typecheck(env, fname, fenv, diag);

        if (arg_ty == nullptr)
            return nullptr;

        if ((*arg_ty) != (*fenv[func_name][i + 1])) {
            std::string err = "Expected type: " + fenv[func_name][i+1]->get_source_type() +
                "for array initialization";
            diag->print_error(func_args[i]->sr, err.c_str());
            return nullptr;
        }
    }

    return ret_type;
}
