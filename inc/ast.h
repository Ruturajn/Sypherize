#ifndef __AST_H__
#define __AST_H__

#include <memory>
#include <ostream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "./diagnostics.h"

class Type;

using Environment =
    std::unordered_map<std::string, std::unordered_map<std::string, Type*>>;

using FuncEnvironment =
    std::unordered_map<std::string, std::vector<Type*>>;

enum class BinopType {
    BINOP_PLUS,
    BINOP_MINUS,
    BINOP_MULT,
    BINOP_DIVIDE,
    BINOP_MODULUS,
    BINOP_LSHIFT,
    BINOP_RSHIFT,
    BINOP_BITAND,
    BINOP_BITOR,
    BINOP_BITXOR,
    BINOP_LT,
    BINOP_LTE,
    BINOP_GT,
    BINOP_GTE,
    BINOP_EQEQUAL,
    BINOP_NEQUAL,
    BINOP_LOGAND,
    BINOP_LOGOR
};

enum class UnopType {
    UNOP_NEG,
    UNOP_NOT,
    UNOP_DEREF,
    UNOP_ADDROF
};

///===-------------------------------------------------------------------===///
/// Types
///===-------------------------------------------------------------------===///
class Type {
public:
    bool is_indexable;
    bool is_index;
    bool is_void;
    Type() : is_indexable(false), is_index(false), is_void(false) {};
    virtual ~Type() = default;
    virtual void print_type() const = 0;
    virtual std::string get_source_type() const = 0;
    virtual bool operator==(const Type& other) const = 0;
    bool operator!=(const Type& other) const { return !operator==(other); }
    virtual Type* get_underlying_type() const = 0;
    virtual bool is_valid_binop(BinopType b) const = 0;
    virtual bool is_valid_unop(UnopType u) const = 0;
};

class TInt : public Type {
public:
    TInt() { is_index = true; }
    void print_type() const override { std::cout << "[TInt]"; }
    std::string get_source_type() const override { return "int"; }
    bool operator==(const Type& other) const override {
        return typeid(*this) == typeid(other);
    }
    Type* get_underlying_type() const override { return nullptr; }
    bool is_valid_binop(BinopType b) const override {
        (void)b;
        return true;
    }
    bool is_valid_unop(UnopType u) const override;

};

class TBool : public Type {
public:
    void print_type() const override { std::cout << "[TBool]"; }
    std::string get_source_type() const override { return "bool"; }
    bool operator==(const Type& other) const override {
        return typeid(*this) == typeid(other);
    }
    Type* get_underlying_type() const override { return nullptr; }
    bool is_valid_binop(BinopType b) const override;
    bool is_valid_unop(UnopType u) const override;
};

class TString : public Type {
public:
    void print_type() const override { std::cout << "[TString]"; }
    std::string get_source_type() const override { return "string"; }
    bool operator==(const Type& other) const override {
        return typeid(*this) == typeid(other);
    }
    Type* get_underlying_type() const override { return nullptr; }
    bool is_valid_binop(BinopType b) const override;
    bool is_valid_unop(UnopType u) const override {
        (void)u;
        return false;
    }
};

class TVoid : public Type {
public:
    TVoid() { is_void = true; }
    void print_type() const override { std::cout << "[TVoid]"; }
    std::string get_source_type() const override { return "void"; }
    bool operator==(const Type& other) const override {
        return typeid(*this) == typeid(other);
    }
    Type* get_underlying_type() const override { return nullptr; }
    bool is_valid_binop(BinopType b) const override {
        (void)b;
        return false;
    }
    bool is_valid_unop(UnopType u) const override {
        (void)u;
        return false;
    }
};

class TRef : public Type {
private:
    Type *type;

public:
    TRef(Type* _type) : type(_type) { is_indexable = true; }
    ~TRef() { delete type; }
    void print_type() const override {
        std::cout << "[TRef]";
        type->print_type();
    }
    std::string get_source_type() const override;
    bool operator==(const Type& other) const override;
    Type* get_underlying_type() const override { return type; }
    bool is_valid_binop(BinopType b) const override;
    bool is_valid_unop(UnopType u) const override;
};

class TArray : public Type {
private:
    Type *type;

public:
    TArray(Type* _type) : type(_type) { is_indexable = true; }
    ~TArray() { delete type; }
    void print_type() const override {
        std::cout << "[TArray]";
        type->print_type();
    }
    std::string get_source_type() const override;
    bool operator==(const Type& other) const override;
    Type* get_underlying_type() const override { return type; }
    bool is_valid_binop(BinopType b) const override {
        (void)b;
        return false;
    }
    bool is_valid_unop(UnopType u) const override;
};

///===-------------------------------------------------------------------===///
/// Expressions
///===-------------------------------------------------------------------===///

class ExpNode {
public:
    bool is_indirect;
    SRange sr;
    ExpNode(const SRange& _sr) : is_indirect(false), sr(_sr) {}
    virtual ~ExpNode() = default;
    virtual void print_node(int indent) const = 0;
    virtual Type* typecheck(Environment& env, const std::string& fname,
                            FuncEnvironment& fenv, Diagnostics* diag) const = 0;
};

class NumberExpNode : public ExpNode {
private:
    long val;

public:
    NumberExpNode(long _val, const SRange& _sr) : ExpNode(_sr), val(_val) {}
    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class StringExpNode : public ExpNode {
private:
    std::string val;

public:
    StringExpNode(const std::string& _val, const SRange& _sr)
        : ExpNode(_sr), val(_val) {}
    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class BoolExpNode : public ExpNode {
private:
    bool val;

public:
    BoolExpNode(bool _val, const SRange& _sr)
        : ExpNode(_sr), val(_val) {}
    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class IdExpNode : public ExpNode {
private:
    std::string val;

public:
    IdExpNode(const std::string& _val, const SRange& _sr)
        : ExpNode(_sr), val(_val) {}
    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class CArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty; // Type of elements.
    std::vector<ExpNode*> exp_list;

public:
    CArrExpNode(std::unique_ptr<Type> _ty,
                std::vector<ExpNode*>& _exp_list, const SRange& _sr)
        : ExpNode(_sr), ty(std::move(_ty)), exp_list(_exp_list) {
            is_indirect =  true;
    }

    ~CArrExpNode() {
        for (auto& elem: exp_list)
            delete elem;
    }

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class NewExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::unique_ptr<ExpNode> exp; // If nullptr then just one elem of primitive
                                  // type.

public:
    NewExpNode(std::unique_ptr<Type> _ty,
               std::unique_ptr<ExpNode> _exp, const SRange& _sr)
        : ExpNode(_sr), ty (std::move(_ty)), exp(std::move(_exp)) {
            is_indirect = true;
    }

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class IndexExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> exp;
    std::unique_ptr<ExpNode> idx;

public:
    IndexExpNode(std::unique_ptr<ExpNode> _exp,
                  std::unique_ptr<ExpNode> _idx, const SRange& _sr)
        : ExpNode(_sr), exp(std::move(_exp)), idx(std::move(_idx)) {}

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class BinopExpNode : public ExpNode {
public:

private:
    enum BinopType binop;
    std::unique_ptr<ExpNode> left, right;

public:
    BinopExpNode(enum BinopType _binop, std::unique_ptr<ExpNode> _left,
                 std::unique_ptr<ExpNode> _right, const SRange& _sr)
        : ExpNode(_sr), binop(_binop), left(std::move(_left)),
          right(std::move(_right)) {}

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class UnopExpNode : public ExpNode {
public:

private:
    enum UnopType uop;
    std::unique_ptr<ExpNode> exp;

public:
    UnopExpNode(enum UnopType _uop, std::unique_ptr<ExpNode> _exp,
                const SRange& _sr)
        : ExpNode(_sr), uop(_uop), exp(std::move(_exp)) {}

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class FunCallExpNode : public ExpNode {
private:
    std::string func_name;
    std::vector<ExpNode*> func_args;

public:
    FunCallExpNode(const std::string& _func_name,
                    std::vector<ExpNode*>& _func_args, const SRange& _sr)
        : ExpNode(_sr), func_name(_func_name), func_args(_func_args) {}

    ~FunCallExpNode() {
        for (auto& elem: func_args)
            delete elem;
    }

    void print_node(int indent) const override;
    Type* typecheck(Environment& env, const std::string& fname,
                    FuncEnvironment& fenv, Diagnostics* diag) const override;
};

///===-------------------------------------------------------------------===///
/// Statements
///===-------------------------------------------------------------------===///

class StmtNode {
public:
    SRange sr;
    StmtNode(const SRange& _sr) : sr(_sr) {}
    virtual ~StmtNode() = default;
    virtual void print_stmt(int indent) const = 0;
    virtual bool typecheck(Environment& env, const std::string& fname,
                           FuncEnvironment& fenv, Diagnostics* diag) const = 0;
};

class AssnStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> lhs, rhs;

public:
    AssnStmtNode(std::unique_ptr<ExpNode> _lhs, std::unique_ptr<ExpNode> _rhs,
                 const SRange& _sr)
        : StmtNode(_sr), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class DeclStmtNode : public StmtNode {
private:
    std::unique_ptr<Type> ty;
    std::string id;
    std::unique_ptr<ExpNode> exp;

public:
    DeclStmtNode(std::unique_ptr<Type> _ty,
                 const std::string& _id,
                 std::unique_ptr<ExpNode> _exp,
                 const SRange& _sr)
        : StmtNode(_sr), ty(std::move(_ty)), id(_id), exp(std::move(_exp)) {}

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class SCallStmtNode : public StmtNode {
private:
    std::string fname;
    std::vector<ExpNode*> fargs;

public:
    SCallStmtNode(const std::string& _fname,
                  std::vector<ExpNode*>& _fargs,
                  const SRange& _sr)
        : StmtNode(_sr), fname(_fname), fargs(_fargs) {}

    ~SCallStmtNode() {
        for (auto& elem: fargs)
            delete elem;
    }

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class RetStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> exp;

public:
    RetStmtNode(std::unique_ptr<ExpNode> _exp, const SRange& _sr)
        : StmtNode(_sr), exp(std::move(_exp)) {}

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class IfStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<StmtNode*> then_body;
    std::vector<StmtNode*> else_body;

public:
    IfStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<StmtNode*>& _then_body,
                std::vector<StmtNode*>& _else_body,
                const SRange& _sr)
        : StmtNode(_sr), cond(std::move(_cond)), then_body(_then_body),
            else_body(_else_body) {}

    ~IfStmtNode() {
        for (auto& elem: then_body)
            delete elem;

        for (auto& elem: else_body)
            delete elem;
    }

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class ForStmtNode : public StmtNode {
private:
    std::vector<StmtNode*> decl_list;
    std::unique_ptr<ExpNode> cond;
    std::unique_ptr<StmtNode> iter;
    std::vector<StmtNode*> body;

public:
    ForStmtNode(std::vector<StmtNode*>& _decl_list,
                std::unique_ptr<ExpNode> _cond,
                std::unique_ptr<StmtNode> _iter,
                std::vector<StmtNode*>& _body,
                const SRange& _sr)
        : StmtNode(_sr), decl_list(_decl_list), cond(std::move(_cond)),
            iter(std::move(_iter)), body(_body) {}

    ~ForStmtNode() {
        for (auto& elem: decl_list)
            delete elem;

        for (auto& elem: body)
            delete elem;
    }

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

class WhileStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<StmtNode*> body;

public:
    WhileStmtNode(std::unique_ptr<ExpNode> _cond,
                  std::vector<StmtNode*>& _body,
                  const SRange& _sr)
        : StmtNode(_sr), cond(std::move(_cond)), body(_body) {}

    ~WhileStmtNode() {
        for (auto& elem: body)
            delete elem;
    }

    void print_stmt(int indent) const override;
    bool typecheck(Environment& env, const std::string& fname,
                   FuncEnvironment& fenv, Diagnostics* diag) const override;
};

///===-------------------------------------------------------------------===///
/// Top Level Declarations
///===-------------------------------------------------------------------===///
class Decls {
public:
    SRange sr;
    Decls(const SRange& _sr) : sr(_sr) {}
    virtual ~Decls() = default;
    virtual void print_decl(int indent) const = 0;
    virtual bool typecheck(Environment& env, FuncEnvironment& fenv,
                           Diagnostics* diag) const = 0;
};

class FunDecl : public Decls {
public:
    std::unique_ptr<Type> frtype;
    std::string fname;
    std::vector<std::pair<Type*, std::string>> args;
    std::vector<StmtNode*> block;

    FunDecl(std::unique_ptr<Type> _frtype,
            const std::string& _fname,
            std::vector<std::pair<Type*, std::string>>& _args,
            std::vector<StmtNode*>& _block,
            const SRange& _sr)
        : Decls(_sr), frtype(std::move(_frtype)), fname(_fname),
            args(_args), block(_block) {}

    ~FunDecl() {
        for (auto& elem: block)
            delete elem;

        for (auto& arg: args)
            delete arg.first;
    }

    void print_decl(int indent) const override;
    bool typecheck(Environment& env, FuncEnvironment& fenv,
                   Diagnostics* diag) const override;
};

class GlobalDecl : public Decls {
public:
    std::unique_ptr<Type> ty;
    std::string id;
    std::unique_ptr<ExpNode> exp;

    GlobalDecl(std::unique_ptr<Type> _ty,
               const std::string& _id,
               std::unique_ptr<ExpNode> _exp,
               const SRange& _sr)
        : Decls(_sr), ty (std::move(_ty)), id (_id), exp(std::move(_exp)) {}

    void print_decl(int indent) const override;
    bool typecheck(Environment& env, FuncEnvironment& fenv,
                   Diagnostics* diag) const override;
};

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///
class Program {
public:
    std::vector<Decls*> decl_list;

    Program() : decl_list({}) {}

    ~Program() {
        for (auto& elem: decl_list)
            delete elem;
    }

    void print_prog() const;
    bool typecheck(Environment& env, FuncEnvironment& fenv,
                   Diagnostics *diag) const;
};

#endif // __AST_H__
