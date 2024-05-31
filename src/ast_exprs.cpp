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

    auto num_op = std::make_unique<LLOConst>(this->val);
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
    out.first.second = std::make_unique<LLONull>();

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

    // TODO: Complile IdExpNode as LHS for supporting structs.
    (void)is_lhs;

    if (ctxt.find(this->val) == ctxt.end()) {
        diag->print_error(sr, "[ICE] Encountered unknown identifier "
                "during compilation");
        return false;
    }

    out.first.first = ctxt[this->val].first;


    auto res_uid = gentemp_ll("id_exp");
    out.first.second = std::make_unique<LLOId>(res_uid);

    std::unique_ptr<LLType> load_ty(new LLTPtr(ctxt[this->val].first->clone()));
    auto load_insn = std::make_unique<LLILoad>(std::move(load_ty),
        std::move(ctxt[this->val].second->clone()));

    auto elem = new LLEInsn(res_uid, std::move(load_insn));
    out.second->stream.push_back(elem);

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

    if (exp_list.size() == 0) {
        diag->print_error(sr, "Expected initialization elements for array initializer"
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

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile CArrExpNode as LHS");
        return false;
    }

    // Refer to llvm/array.ll
    // Add 1 to account for storing the array size.
    ssize_t arr_size = this->exp_list.size();

    // Emit alloca
    std::unique_ptr<LLType> ll_ty(this->ty->compile_type());
    auto alloca_uid = gentemp_ll("alloca_carr");
    auto alloca_insn = std::make_unique<LLIAlloca>(
        std::make_unique<LLTArray>(arr_size, std::move(ll_ty)),
        ""
    );

    out.second->stream.push_back(
        new LLEInsn(alloca_uid, std::move(alloca_insn))
    );

    // Store size
    auto gep_uid_sz = gentemp_ll("alloca_gep");
    std::unique_ptr<LLType> gep_ty_sz(this->ty->get_underlying_type()->compile_type());

    auto gep_insn_sz = new LLIGep(
        std::move(gep_ty_sz),
        std::make_unique<LLOId>(gep_uid_sz),
        {new LLOConst(0), new LLOConst(0)}
    );

    std::unique_ptr<LLInsn> gep_insn_ptr_sz(gep_insn_sz);
    out.second->stream.push_back(
        new LLEInsn(gep_uid_sz, std::move(gep_insn_ptr_sz))
    );

    auto store_uid_sz = gentemp_ll("store");
    auto store_ty_sz = std::make_unique<LLTi64>();
    auto store_insn_sz = std::make_unique<LLIStore>(
        std::move(store_ty_sz),
        std::make_unique<LLOConst>(arr_size),
        std::make_unique<LLOId>(gep_uid_sz)
    );
    out.second->stream.push_back(
        new LLEInsn(store_uid_sz, std::move(store_insn_sz))
    );

    // Then repeat gep and store until arr_size
    LLType* res_ty = nullptr;
    std::string& res_op = store_uid_sz;

    for (int i = 0; i < (int)arr_size; i++) {
        auto gep_uid = gentemp_ll("alloca_gep");
        std::unique_ptr<LLType> gep_ty(this->ty->get_underlying_type()->compile_type());

        auto gep_insn = new LLIGep(
            std::move(gep_ty),
            std::make_unique<LLOId>(gep_uid),
            {new LLOConst(0), new LLOConst(1), new LLOConst(i)}
        );
        std::unique_ptr<LLInsn> gep_insn_ptr(gep_insn);
        out.second->stream.push_back(
            new LLEInsn(gep_uid, std::move(gep_insn_ptr))
        );

        auto store_uid = gentemp_ll("store");
        if (this->exp_list[i]->compile(ctxt, out, diag, false)) {
            diag->print_error(exp_list[i]->sr, "[ICE] Unable to compile expression");
            return false;
        }
        auto store_ty = out.first.first->clone();
        auto store_insn = std::make_unique<LLIStore>(
            std::move(store_ty),
            std::move(out.first.second->clone()),
            std::make_unique<LLOId>(gep_uid)
        );
        out.second->stream.push_back(
            new LLEInsn(store_uid, std::move(store_insn))
        );

        res_ty = store_ty.get();
        res_op = store_uid;
    }

    out.first.first = res_ty;
    out.first.second = std::make_unique<LLOId>(res_op);

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

bool NewExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                         bool is_lhs) const{

    (void)ctxt;
    (void)out;

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Can't compile NewExpNode as LHS");
        return false;
    }

    // TODO: Develop runtime to allocate memory on the heap.

    /* ssize_t size_list = this->exp_list.size(); */

    // Emit instructions to allocate the right amount of memory on the heap.
    /* if (size_list == 0) */

    diag->print_error(sr, "[ICE] NewExpNode unimplemented");
    return false;
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

bool IndexExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                           bool is_lhs) const{

    // TODO: Develop runtime to check for array index out of range.
    // TODO: Implement indexing into multi-dimensional arrays.

    if (this->exp->compile(ctxt, out, diag, false) == false) {
        diag->print_error(exp->sr, "[ICE] Unable to compile expression");
        return false;
    }

    ssize_t idx_list_size = this->idx_list.size();

    if (idx_list_size > 1) {
        diag->print_error(sr, "[ICE] Multi-dimensional array indexing unimplemented");
        return false;
    }

    // Gep to get a pointer for a particular index.
    auto gep_ty = out.first.first->clone();
    auto gep_op = out.first.second->clone();

    if (this->idx_list[0]->compile(ctxt, out, diag, false) == false) {
        diag->print_error(idx_list[0]->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto idx_op = out.first.second->clone();

    auto gep_insn = new LLIGep(
        std::move(gep_ty),
        std::move(gep_op),
        {new LLOConst(0), new LLOConst(1), idx_op.get()}
    );
    std::unique_ptr<LLInsn> gep_insn_ptr(gep_insn);
    auto gep_uid = gentemp_ll("index_gep");
    out.second->stream.push_back(
        new LLEInsn(gep_uid, std::move(gep_insn_ptr))
    );

    // If `is_lhs` is set, then return without the load instruction.
    if (is_lhs) {
        out.first.first = gep_ty.get();
        out.first.second = gep_op->clone();
        return true;
    }

    auto res_uid = gentemp_ll("index_load");

    if (gep_ty->get_underlying_type() == nullptr) {
        diag->print_error(sr, "[ICE] Unable to index into expression, due to"
                              " base type not being indexable");
        return false;
    }

    auto load_ty = gep_ty->get_underlying_type()->clone();

    auto load_insn = std::make_unique<LLILoad>(std::move(load_ty),
        std::move(gep_op->clone()));

    out.second->stream.push_back(new LLEInsn(res_uid, std::move(load_insn)));
    out.first.first = load_ty.get();
    out.first.second = std::make_unique<LLOId>(res_uid);

    return true;
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

std::unique_ptr<LLType> BinopExpNode::ll_return_bop_type() const {
    switch (this->binop) {
        case BinopType::BINOP_PLUS:
        case BinopType::BINOP_MINUS:
        case BinopType::BINOP_MULT:
        case BinopType::BINOP_DIVIDE:
        case BinopType::BINOP_MODULUS:
        case BinopType::BINOP_LSHIFT:
        case BinopType::BINOP_RSHIFT:
        case BinopType::BINOP_BITAND:
        case BinopType::BINOP_BITOR:
        case BinopType::BINOP_BITXOR:
            return std::make_unique<LLTi64>();

        case BinopType::BINOP_LT:
        case BinopType::BINOP_LTE:
        case BinopType::BINOP_GT:
        case BinopType::BINOP_GTE:
        case BinopType::BINOP_EQEQUAL:
        case BinopType::BINOP_NEQUAL:
        case BinopType::BINOP_LOGAND:
        case BinopType::BINOP_LOGOR:
            return std::make_unique<LLTi1>();

        default:
            return nullptr;
    }
}

bool BinopExpNode::ll_is_cond() const {
    switch (this->binop) {
        case BinopType::BINOP_PLUS:
        case BinopType::BINOP_MINUS:
        case BinopType::BINOP_MULT:
        case BinopType::BINOP_DIVIDE:
        case BinopType::BINOP_MODULUS:
        case BinopType::BINOP_LSHIFT:
        case BinopType::BINOP_RSHIFT:
        case BinopType::BINOP_BITAND:
        case BinopType::BINOP_BITOR:
        case BinopType::BINOP_BITXOR:
        case BinopType::BINOP_LOGAND:
        case BinopType::BINOP_LOGOR:
            return false;

        case BinopType::BINOP_LT:
        case BinopType::BINOP_LTE:
        case BinopType::BINOP_GT:
        case BinopType::BINOP_GTE:
        case BinopType::BINOP_EQEQUAL:
        case BinopType::BINOP_NEQUAL:
            return true;

        default:
            return false;
    }
}

bool BinopExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                           bool is_lhs) const {
    if (is_lhs) {
        diag->print_error(sr, "[ICE] Unable to compile BinopExpNode as LHS");
        return false;
    }

    if (this->left->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->left->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto lhs_ty = out.first.first->clone();
    auto lhs_op = out.first.second->clone();

    if (this->right->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->right->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto rhs_ty = out.first.first->clone();
    auto rhs_op = out.first.second->clone();
    auto bop_ty = this->ll_return_bop_type();

    if (!this->ll_is_cond()) {
        LLBinopType ll_bop = LLBinopType::LLBINOP_ADD;

        switch (this->binop) {
            case BinopType::BINOP_PLUS:
                ll_bop = LLBinopType::LLBINOP_ADD;
                break;
            case BinopType::BINOP_MINUS:
                ll_bop = LLBinopType::LLBINOP_SUB;
                break;
            case BinopType::BINOP_MULT:
                ll_bop = LLBinopType::LLBINOP_MULT;
                break;
            case BinopType::BINOP_DIVIDE:
                ll_bop = LLBinopType::LLBINOP_DIVIDE;
                break;
            case BinopType::BINOP_MODULUS:
                ll_bop = LLBinopType::LLBINOP_REM;
                break;
            case BinopType::BINOP_LSHIFT:
                ll_bop = LLBinopType::LLBINOP_LSHIFT;
                break;
            case BinopType::BINOP_RSHIFT:
                ll_bop = LLBinopType::LLBINOP_RSHIFT;
                break;
            case BinopType::BINOP_BITAND:
                ll_bop = LLBinopType::LLBINOP_BITAND;
                break;
            case BinopType::BINOP_BITOR:
                ll_bop = LLBinopType::LLBINOP_BITOR;
                break;
            case BinopType::BINOP_BITXOR:
                ll_bop = LLBinopType::LLBINOP_BITXOR;
                break;
            case BinopType::BINOP_LOGAND:
                ll_bop = LLBinopType::LLBINOP_BITAND;
                break;
            case BinopType::BINOP_LOGOR:
                ll_bop = LLBinopType::LLBINOP_BITOR;
                break;
            default:
                break;
        }

        auto res_uid = gentemp_ll("bop");
        out.first.first = bop_ty.get();
        out.first.second = std::make_unique<LLOId>(res_uid);

        auto binop_insn = std::make_unique<LLIBinop>(ll_bop, std::move(bop_ty),
            std::move(lhs_op), std::move(rhs_op));

        out.second->stream.push_back(new LLEInsn(res_uid, std::move(binop_insn)));

    } else {
        LLCondType ll_cond = LLCondType::LLCOND_EQUAL;

        switch (this->binop) {
            case BinopType::BINOP_LT:
                ll_cond = LLCondType::LLCOND_LT;
                break;
            case BinopType::BINOP_LTE:
                ll_cond = LLCondType::LLCOND_LTE;
                break;
            case BinopType::BINOP_GT:
                ll_cond = LLCondType::LLCOND_GT;
                break;
            case BinopType::BINOP_GTE:
                ll_cond = LLCondType::LLCOND_GTE;
                break;
            case BinopType::BINOP_EQEQUAL:
                ll_cond = LLCondType::LLCOND_EQUAL;
                break;
            case BinopType::BINOP_NEQUAL:
                ll_cond = LLCondType::LLCOND_NEQUAL;
                break;
            default:
                break;
        }

        auto res_uid = gentemp_ll("icmp");
        out.first.first = bop_ty.get();
        out.first.second = std::make_unique<LLOId>(res_uid);

        auto icmp_insn = std::make_unique<LLIIcmp>(ll_cond, std::move(bop_ty),
            std::move(lhs_op), std::move(rhs_op));

        out.second->stream.push_back(new LLEInsn(res_uid, std::move(icmp_insn)));
    }

    return true;
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

bool UnopExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                           bool is_lhs) const {
    if (is_lhs) {
        diag->print_error(sr, "[ICE] Unable to compile UnopExpNode as LHS");
        return false;
    }

    if (this->exp->compile(ctxt, out, diag, false) == false) {
        diag->print_error(this->exp->sr, "[ICE] Unable to compile expression");
        return false;
    }

    auto exp_ty = out.first.first->clone();
    auto exp_op = out.first.second->clone();

    auto res_uid = gentemp_ll("unop");
    LLInsn* insn = nullptr;

    switch (this->uop) {
        case UnopType::UNOP_NEG: {
            auto insn_ty = std::make_unique<LLTi64>();
            out.first.first = insn_ty.get();
            insn = new LLIBinop(LLBinopType::LLBINOP_BITXOR,
                                std::move(insn_ty),
                                std::move(exp_op),
                                std::make_unique<LLOConst>(-1));
            break;
        }

        case UnopType::UNOP_NOT: {
            auto insn_ty = std::make_unique<LLTi1>();
            out.first.first = insn_ty.get();
            insn = new LLIIcmp(LLCondType::LLCOND_EQUAL,
                                std::move(insn_ty),
                                std::move(exp_op),
                                std::make_unique<LLOConst>(1));
            break;
        }

        case UnopType::UNOP_DEREF: {
            auto insn_ty = exp_ty->get_underlying_type()->clone();
            out.first.first = insn_ty.get();

            auto load_ty = insn_ty->clone();
            insn = new LLILoad(
                std::move(load_ty),
                std::move(exp_op->clone())
            );

            break;
        }

        case UnopType::UNOP_ADDROF: {
            auto insn_ty = std::make_unique<LLTPtr>(std::move(exp_ty->clone()));
            out.first.first = insn_ty.get();

            auto alloca_uid = gentemp_ll("alloca_unop");
            auto alloca_insn = std::make_unique<LLIAlloca>(
                std::move(insn_ty->clone()), ""
            );
            out.second->stream.push_back(new LLEInsn(alloca_uid, std::move(alloca_insn)));

            auto store_uid = gentemp_ll("store");
            insn = new LLIStore(
                std::move(exp_ty->clone()),
                std::move(exp_op->clone()),
                std::make_unique<LLOId>(alloca_uid)
            );

            break;
        }
    }

    out.first.second = std::make_unique<LLOId>(res_uid);

    std::unique_ptr<LLInsn> insn_ptr(insn);
    out.second->stream.push_back(new LLEInsn(res_uid, std::move(insn_ptr)));

    return true;
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

bool FunCallExpNode::compile(LLCtxt& ctxt, LLOut& out, Diagnostics* diag,
                             bool is_lhs) const {

    if (is_lhs) {
        diag->print_error(sr, "[ICE] Unable to compile FunCallExpNode as LHS");
        return false;
    }

    std::vector<std::pair<LLType*, LLOperand*>> ty_arg_list {};

    for (auto arg: this->func_args) {
        if (arg->compile(ctxt, out, diag, false) == false) {
            diag->print_error(arg->sr, "[ICE] Unable to compile expression");
            return false;
        }
        ty_arg_list.push_back({out.first.first->clone().release(),
                out.first.second->clone().release()});
    }

    if (ctxt.find(this->func_name) ==  ctxt.end()) {
        diag->print_error(sr, "[ICE] Call to unknown function during compilation");
        return false;
    }

    if (ctxt[this->func_name].first->get_underlying_type() == nullptr) {
        diag->print_error(sr, "[ICE] Expected pointer type during"
                            " compiling a FunCallExpNode");
        return false;
    }

    auto call_ty_raw = ctxt[this->func_name].first->get_underlying_type()->get_return_type();

    if (call_ty_raw == nullptr) {
        diag->print_error(sr, "[ICE] Expected pointer to a function type during"
                            " compiling a FunCallExpNode");
        return false;
    }

    auto call_ty = call_ty_raw->clone();

    out.first.first = call_ty.get();

    auto call_uid = gentemp_ll("funcall");
    auto call_insn = std::make_unique<LLICall>(
        std::move(call_ty),
        std::make_unique<LLOGid>(this->func_name),
        ty_arg_list
    );

    out.first.second = std::make_unique<LLOId>(call_uid);
    out.second->stream.push_back(new LLEInsn(call_uid, std::move(call_insn)));

    return true;
}
