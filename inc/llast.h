#ifndef __LLAST_H__
#define __LLAST_H__

#include <memory>
#include <ostream>
#include <string>
#include <iostream>
#include <vector>

enum class LLBinopType {
    LLBINOP_ADD,
    LLBINOP_SUB,
    LLBINOP_MULT,
    LLBINOP_DIVIDE,
    LLBINOP_REM,
    LLBINOP_LSHIFT,
    LLBINOP_RSHIFT,
    LLBINOP_BITAND,
    LLBINOP_BITOR,
    LLBINOP_BITXOR,
};

enum class LLCondType {
    LLCOND_EQUAL,
    LLCOND_NEQUAL,
    LLCOND_LT,
    LLCOND_LTE,
    LLCOND_GT,
    LLCOND_GTE,
};


///===-------------------------------------------------------------------===///
/// LLVM IR Uids
///===-------------------------------------------------------------------===///
std::string gentemp_ll(const std::string& base);

///===-------------------------------------------------------------------===///
/// LLVM IR Types
///===-------------------------------------------------------------------===///

class LLType {
public:
    LLType() = default;
    virtual ~LLType() = default;
    virtual void print_ll_type(std::ostream& os) const = 0;
    virtual bool operator==(const LLType& other) const = 0;
    bool operator!=(const LLType& other) const { return !operator==(other); }
    virtual LLType* get_underlying_type() const = 0;
    virtual std::unique_ptr<LLType> clone() const = 0;
};

class LLTVoid : public LLType {
public:
    void print_ll_type(std::ostream& os) const override { os << "void"; }
    bool operator==(const LLType& other) const override {
        return typeid(*this) == typeid(other);
    }
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTVoid>();
    }
};

class LLTi1 : public LLType {
public:
    void print_ll_type(std::ostream& os) const override { os << "i1"; }
    bool operator==(const LLType& other) const override {
        return typeid(*this) == typeid(other);
    }
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTi1>();
    }
};

class LLTi8 : public LLType {
public:
    void print_ll_type(std::ostream& os) const override { os << "i8"; }
    bool operator==(const LLType& other) const override {
        return typeid(*this) == typeid(other);
    }
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTi8>();
    }
};

class LLTi64 : public LLType {
public:
    void print_ll_type(std::ostream& os) const override { os << "i64"; }
    bool operator==(const LLType& other) const override {
        return typeid(*this) == typeid(other);
    }
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTi64>();
    }
};

class LLTPtr : public LLType {
private:
    std::unique_ptr<LLType> ty;

public:
    LLTPtr(std::unique_ptr<LLType> _ty) : ty(std::move(_ty)) {}
    void print_ll_type(std::ostream& os) const override;
    bool operator==(const LLType& other) const override;
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTPtr>(ty->clone());
    }
};

class LLTStruct : public LLType {
private:
    std::vector<LLType*> ty_list;

public:
    LLTStruct(std::vector<LLType*>& _ty_list) : ty_list(_ty_list) {}

    ~LLTStruct() {
        for (auto& elem: ty_list)
            delete elem;
    }

    void print_ll_type(std::ostream& os) const override;
    bool operator==(const LLType& other) const override;
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        std::vector<LLType*> clone_ty_list {};

        for (auto elem: ty_list)
            clone_ty_list.push_back(elem->clone().release());

        return std::make_unique<LLTStruct>(clone_ty_list);
    }
};

class LLTArray : public LLType {
private:
    ssize_t arr_len;
    std::unique_ptr<LLType> arr_ty;

public:
    LLTArray(ssize_t _arr_len, std::unique_ptr<LLType> _arr_ty)
        : arr_len(_arr_len), arr_ty(std::move(_arr_ty)) {}
    void print_ll_type(std::ostream& os) const override;
    bool operator==(const LLType& other) const override;
    LLType* get_underlying_type() const override { return arr_ty.get(); }
    std::unique_ptr<LLType> clone() const override {
        return std::make_unique<LLTArray>(arr_len, arr_ty->clone());
    }
};

class LLTFunc : public LLType {
private:
    std::vector<LLType*> arg_ty_list;
    std::unique_ptr<LLType> ret_ty;

public:
    LLTFunc(std::vector<LLType*>& _arg_ty_list, std::unique_ptr<LLType> _ret_ty)
        : arg_ty_list(_arg_ty_list), ret_ty(std::move(_ret_ty)) {}

    ~LLTFunc() {
        for (auto& elem: arg_ty_list)
            delete elem;
    }

    void print_ll_type(std::ostream& os) const override;
    bool operator==(const LLType& other) const override;
    LLType* get_underlying_type() const override { return nullptr; }
    std::unique_ptr<LLType> clone() const override {
        std::vector<LLType*> clone_arg_ty_list {};

        for (auto elem: arg_ty_list)
            clone_arg_ty_list.push_back(elem->clone().release());

        return std::make_unique<LLTFunc>(clone_arg_ty_list, ret_ty->clone());
    }
};

class LLTNamed : public LLType {
private:
    std::string ty_name;

public:
    LLTNamed(const std::string& _ty_name) : ty_name(_ty_name) {}
    void print_ll_type(std::ostream& os) const override;
    bool operator==(const LLType& other) const override;
    LLType* get_underlying_type() const override { return nullptr; }
};

///===-------------------------------------------------------------------===///
/// LLVM IR Operands
///===-------------------------------------------------------------------===///

class LLOperand {
public:
    LLOperand() = default;
    virtual ~LLOperand() = default;
    virtual void print_ll_op(std::ostream& os) const = 0;
};

class LLONull : public LLOperand {
public:
    void print_ll_op(std::ostream& os) const override { os << "null"; }
};

class LLOConst : public LLOperand {
private:
    ssize_t val;

public:
    LLOConst(ssize_t _val) : val(_val) {}
    void print_ll_op(std::ostream& os) const override {
        os << val;
    }
};

class LLOGid : public LLOperand {
private:
    std::string id;

public:
    LLOGid(const std::string& _id) : id(_id) {}
    void print_ll_op(std::ostream& os) const override { os << id; }
};

class LLOId : public LLOperand {
private:
    std::string uid;

public:
    LLOId(const std::string& _uid) : uid(_uid) {}
    void print_ll_op(std::ostream& os) const override { os << uid; }
};

///===-------------------------------------------------------------------===///
/// LLVM IR Instructions
///===-------------------------------------------------------------------===///

class LLInsn {
public:
    LLInsn() = default;
    virtual ~LLInsn() = default;
    virtual void print_ll_insn(std::ostream& os) const = 0;
};

class LLIBinop : public LLInsn {
private:
    LLBinopType bop;
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op1;
    std::unique_ptr<LLOperand> op2;

public:
    LLIBinop(LLBinopType _bop, std::unique_ptr<LLType> _ty,
             std::unique_ptr<LLOperand> _op1, std::unique_ptr<LLOperand> _op2)
        : bop(_bop), ty(std::move(_ty)), op1(std::move(_op1)), op2(std::move(_op2)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLIAlloca : public LLInsn {
private:
    std::unique_ptr<LLType> ty;

public:
    LLIAlloca(std::unique_ptr<LLType> _ty) : ty(std::move(_ty)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLILoad : public LLInsn {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op;

public:
    LLILoad(std::unique_ptr<LLType> _ty, std::unique_ptr<LLOperand> _op)
        : ty(std::move(_ty)), op(std::move(_op)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLIStore : public LLInsn {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op_from;
    std::unique_ptr<LLOperand> op_to;

public:
    LLIStore(std::unique_ptr<LLType> _ty,
             std::unique_ptr<LLOperand> _op_from,
             std::unique_ptr<LLOperand> _op_to)
        : ty(std::move(_ty)), op_from(std::move(_op_from)),
                op_to(std::move(_op_to)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLIIcmp : public LLInsn {
private:
    LLCondType cond;
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op1;
    std::unique_ptr<LLOperand> op2;

public:
    LLIIcmp(LLCondType _cond,
             std::unique_ptr<LLType> _ty,
             std::unique_ptr<LLOperand> _op1,
             std::unique_ptr<LLOperand> _op2)
        : cond(_cond), ty(std::move(_ty)), op1(std::move(_op1)),
                op2(std::move(_op2)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLICall : public LLInsn {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op;
    std::vector<std::pair<LLType*, LLOperand*>> ty_arg_list;

public:
    LLICall(std::unique_ptr<LLType> _ty,
            std::unique_ptr<LLOperand> _op,
            std::vector<std::pair<LLType*, LLOperand*>>& _ty_arg_list)
        : ty(std::move(_ty)), op(std::move(_op)), ty_arg_list(_ty_arg_list) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLIBitcast : public LLInsn {
private:
    std::unique_ptr<LLType> from_ty;
    std::unique_ptr<LLOperand> op;
    std::unique_ptr<LLType> to_ty;

public:
    LLIBitcast(std::unique_ptr<LLType> _from_ty,
               std::unique_ptr<LLOperand> _op,
               std::unique_ptr<LLType> _to_ty)
        : from_ty(std::move(_from_ty)),
            op(std::move(_op)), to_ty(std::move(_to_ty)) {}
    void print_ll_insn(std::ostream& os) const override;
};

class LLIGep : public LLInsn {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op;
    std::vector<LLOperand*> op_list;

public:
    LLIGep(std::unique_ptr<LLType> _ty,
           std::unique_ptr<LLOperand> _op,
           std::vector<LLOperand*>& _op_list)
        : ty(std::move(_ty)), op(std::move(_op)), op_list(_op_list) {}

    LLIGep(std::unique_ptr<LLType> _ty,
           std::unique_ptr<LLOperand> _op,
           const std::initializer_list<LLOperand*>& _op_list)
        : ty(std::move(_ty)), op(std::move(_op)), op_list(_op_list) {}

    ~LLIGep() {
        for (auto& elem: op_list)
            delete elem;
    }

    void print_ll_insn(std::ostream& os) const override;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Terminators
///===-------------------------------------------------------------------===///

class LLTerm {
public:
    LLTerm() = default;
    virtual ~LLTerm() = default;
    virtual void print_ll_term(std::ostream& os) const = 0;
};

class LLTermRet : public LLTerm {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLOperand> op;

public:
    LLTermRet(std::unique_ptr<LLType> _ty, std::unique_ptr<LLOperand> _op)
        : ty(std::move(_ty)), op(std::move(_op)) {}
    void print_ll_term(std::ostream& os) const override;
};

class LLTermBr : public LLTerm {
private:
    std::string lbl;

public:
    LLTermBr(const std::string& _lbl) : lbl(_lbl) {}
    void print_ll_term(std::ostream& os) const override;
};

class LLTermCbr : public LLTerm {
private:
    std::unique_ptr<LLOperand> op;
    std::string true_lbl;
    std::string false_lbl;

public:
    LLTermCbr(std::unique_ptr<LLOperand> _op,
              const std::string& _true_lbl,
              const std::string& _false_lbl)
        : op(std::move(_op)), true_lbl(_true_lbl),
            false_lbl(_false_lbl) {}
    void print_ll_term(std::ostream& os) const override;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Block
///===-------------------------------------------------------------------===///

class LLBlock {
private:
    std::vector<std::pair<std::string, LLInsn*>> insn_list;
    std::pair<std::string, LLTerm*> term;

public:
    LLBlock(std::vector<std::pair<std::string, LLInsn*>>& _insn_list,
            std::pair<std::string, LLTerm*>& _term)
        : insn_list(_insn_list), term(_term) {}

    ~LLBlock() {
        for (auto &elem: insn_list) {
            delete elem.second;
        }

        delete term.second;
    }

    void print_ll_block(std::ostream& os) const;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Control Flow Graph
///===-------------------------------------------------------------------===///

class LLCFG {
private:
    std::pair<LLBlock*, std::vector<std::pair<std::string, LLBlock*>>> cfg;

public:
    LLCFG(std::pair<LLBlock*, std::vector<std::pair<std::string, LLBlock*>>>& _cfg)
        : cfg(_cfg) {}

    ~LLCFG() {
        delete cfg.first;

        for (auto& elem: cfg.second)
            delete elem.second;
    }

    void print_ll_cfg(std::ostream& os) const;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Function Declaration
///===-------------------------------------------------------------------===///

class LLFDecl {
private:
    std::vector<LLType*> func_params_ty;
    std::unique_ptr<LLType> func_ret_ty;
    std::vector<std::string> func_params;
    std::unique_ptr<LLCFG> func_cfg;

public:
    LLFDecl(std::vector<LLType*>& _func_params_ty,
            std::unique_ptr<LLType>& _func_ret_ty,
            std::vector<std::string>& _func_params,
            std::unique_ptr<LLCFG>& _func_cfg)
        : func_params_ty(_func_params_ty), func_ret_ty(std::move(_func_ret_ty)),
            func_params(_func_params), func_cfg(std::move(_func_cfg)) {}

    ~LLFDecl() {
        for (auto& elem: func_params_ty)
            delete elem;
    }

    void print_ll_fdecl(std::ostream& os) const;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Global Initializers
///===-------------------------------------------------------------------===///

class LLGInit {
public:
    LLGInit() = default;
    virtual ~LLGInit() = default;
    virtual void print_ll_ginit(std::ostream& os) const = 0;
};

class LLGNull : public LLGInit {
public:
    void print_ll_ginit(std::ostream& os) const override;
};

class LLGGid : public LLGInit {
private:
    std::string gid;

public:
    LLGGid(const std::string& _gid) : gid(_gid) {}
    void print_ll_ginit(std::ostream& os) const override;
};

class LLGInt : public LLGInit {
private:
    ssize_t val;

public:
    LLGInt(ssize_t _val) : val(_val) {}
    void print_ll_ginit(std::ostream& os) const override;
};

class LLGString : public LLGInit {
private:
    std::string val;

public:
    LLGString(const std::string& _val) : val(_val) {}
    void print_ll_ginit(std::ostream& os) const override;
};

class LLGArray : public LLGInit {
private:
    std::vector<std::pair<LLType*, LLGInit*>> ty_ginit_list;

public:
    LLGArray(std::vector<std::pair<LLType*, LLGInit*>>& _ty_ginit_list)
        : ty_ginit_list(_ty_ginit_list) {}

    ~LLGArray() {
        for (auto& elem: ty_ginit_list) {
            delete elem.first;
            delete elem.second;
        }
    }

    void print_ll_ginit(std::ostream& os) const override;
};

class LLGStruct : public LLGInit {
private:
    std::vector<std::pair<LLType*, LLGInit*>> ty_ginit_list;

public:
    LLGStruct(std::vector<std::pair<LLType*, LLGInit*>>& _ty_ginit_list)
        : ty_ginit_list(_ty_ginit_list) {}

    ~LLGStruct() {
        for (auto& elem: ty_ginit_list) {
            delete elem.first;
            delete elem.second;
        }
    }

    void print_ll_ginit(std::ostream& os) const override;
};

class LLGBitcast : public LLGInit {
private:
    std::unique_ptr<LLType> from_ty;
    std::unique_ptr<LLGInit> ginit;
    std::unique_ptr<LLType> to_ty;

public:
    LLGBitcast(std::unique_ptr<LLType> _from_ty,
               std::unique_ptr<LLGInit> _ginit,
               std::unique_ptr<LLType> _to_ty)
        : from_ty(std::move(_from_ty)), ginit(std::move(_ginit)),
            to_ty(std::move(_to_ty)) {}
    void print_ll_ginit(std::ostream& os) const override;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Global Declaration
///===-------------------------------------------------------------------===///

class LLGDecl {
private:
    std::unique_ptr<LLType> ty;
    std::unique_ptr<LLGInit> ginit;

public:
    LLGDecl(std::unique_ptr<LLType> _ty, std::unique_ptr<LLGInit> _ginit)
        : ty(std::move(_ty)), ginit(std::move(_ginit)) {}
    void print_ll_gdecl(std::ostream& os) const;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Program
///===-------------------------------------------------------------------===///

class LLProg {
private:
    std::vector<std::pair<std::string, LLGDecl*>> gdecls;
    std::vector<std::pair<std::string, LLFDecl*>> fdecls;

public:
    LLProg(std::vector<std::pair<std::string, LLGDecl*>>& _gdecls,
           std::vector<std::pair<std::string, LLFDecl*>>& _fdecls)
        : gdecls(_gdecls), fdecls(_fdecls) {}

    ~LLProg() {
        for (auto& elem: gdecls)
            delete elem.second;

        for (auto& elem: fdecls)
            delete elem.second;
    }

    void print_ll_prog(std::ostream& os) const;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Instruction Stream Elements
///===-------------------------------------------------------------------===///

class LLElt {
public:
    LLElt() = default;
    virtual ~LLElt() = default;
    virtual void print_ll_elt(std::ostream& os) const;
};

class LLELables : public LLElt {
private:
    std::string lbl;

public:
    LLELables(const std::string& _lbl) : lbl(_lbl) {}
    void print_ll_elt(std::ostream& os) const override;
};

class LLEInsn : public LLElt {
private:
    std::string uid;
    std::unique_ptr<LLInsn> insn;

public:
    LLEInsn(const std::string& _uid, std::unique_ptr<LLInsn> _insn)
        : uid(_uid), insn(std::move(_insn)) {}
    void print_ll_elt(std::ostream& os) const override;
};

class LLETerm : public LLElt {
private:
    std::unique_ptr<LLTerm> term;

public:
    LLETerm(std::unique_ptr<LLTerm> _term) : term(std::move(_term)) {}
    void print_ll_elt(std::ostream& os) const override;
};

class LLEGlbl : public LLElt {
private:
    std::string id;
    std::unique_ptr<LLGDecl> gdecl;

public:
    LLEGlbl(const std::string& _gid, std::unique_ptr<LLGDecl> _gdecl)
        : id(_gid), gdecl(std::move(_gdecl)) {}
    void print_ll_elt(std::ostream& os) const override;
};

class LLEEntry : public LLElt {
private:
    std::string id;
    std::unique_ptr<LLInsn> insn;

public:
    LLEEntry(const std::string& _uid, std::unique_ptr<LLInsn> _insn)
        : id(_uid), insn(std::move(_insn)) {}
    void print_ll_elt(std::ostream& os) const override;
};

///===-------------------------------------------------------------------===///
/// LLVM IR Instruction Stream
///===-------------------------------------------------------------------===///

class LLStream {
public:
    std::vector<LLElt*> stream;
    LLStream() : stream({}) {}

    ~LLStream() {
        for (auto& elem: stream)
            delete elem;
    }

    void print_ll_stream() const;
};

#endif // __LLAST_H__
