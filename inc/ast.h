#ifndef __AST_H__
#define __AST_H__

#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>

class TypeChecker;

///===-------------------------------------------------------------------===///
/// Types
///===-------------------------------------------------------------------===///
class Type {
public:
    Type() = default;
    virtual ~Type() = default;
    virtual void print_type() const = 0;
};

class TInt : public Type {
    void print_type() const override { std::cout << "[TInt]"; }
};

class TBool : public Type {
    void print_type() const override { std::cout << "[TBool]"; }
};

class TString : public Type {
    void print_type() const override { std::cout << "[TString]"; }
};

class TVoid : public Type {
    void print_type() const override { std::cout << "[TVoid]"; }
};

class TRef : public Type {
private:
    Type *type;

public:
    TRef(Type* _type) : type(_type) {}
    ~TRef() { delete type; }
    void print_type() const override {
        std::cout << "[TRef]";
        type->print_type();
    }
};

class TArray : public Type {
private:
    Type *type;

public:
    TArray(Type* _type) : type(_type) {}
    ~TArray() { delete type; }
    void print_type() const override {
        std::cout << "[TArray]";
        type->print_type();
    }
};

using Environment = std::unordered_map<std::string, std::unordered_map<std::string, Type*>>;

///===-------------------------------------------------------------------===///
/// Expressions
///===-------------------------------------------------------------------===///

class ExpNode {
public:
    ExpNode() = default;
    virtual ~ExpNode() = default;
    virtual void print_node(int indent) const = 0;
};

class NumberExpNode : public ExpNode {
private:
    long val;

public:
    NumberExpNode(long _val) : val(_val) {}
    void print_node(int indent) const override;
};

class StringExpNode : public ExpNode {
private:
    std::string val;

public:
    StringExpNode(const std::string& _val) : val(_val) {}
    void print_node(int indent) const override;
};

class BoolExpNode : public ExpNode {
private:
    bool val;

public:
    BoolExpNode(bool _val) : val(_val) {}
    void print_node(int indent) const override;
};

class IdExpNode : public ExpNode {
private:
    std::string val;

public:
    IdExpNode(const std::string& _val) : val(_val) {}
    void print_node(int indent) const override;
};

class CArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty; // Type of elements.
    std::vector<ExpNode*> exp_list;

public:
    CArrExpNode(std::unique_ptr<Type> _ty,
                std::vector<ExpNode*>& _exp_list)
        : ty(std::move(_ty)), exp_list(_exp_list) {}

    ~CArrExpNode() {
        for (auto& elem: exp_list)
            delete elem;
    }

    void print_node(int indent) const override;
};

class NewExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::unique_ptr<ExpNode> exp; // If nullptr then just one elem of primitive
                                  // type.

public:
    NewExpNode(std::unique_ptr<Type> _ty,
                  std::unique_ptr<ExpNode> _exp)
        : ty (std::move(_ty)), exp(std::move(_exp)) {}

    void print_node(int indent) const override;
};

class IndexExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> exp;
    std::unique_ptr<ExpNode> idx;

public:
    IndexExpNode(std::unique_ptr<ExpNode> _exp,
                  std::unique_ptr<ExpNode> _idx)
        : exp(std::move(_exp)), idx(std::move(_idx)) {}

    void print_node(int indent) const override;
};

class BinopExpNode : public ExpNode {
public:
    enum BinopType {
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

private:
    enum BinopType binop;
    std::unique_ptr<ExpNode> left, right;

public:
    BinopExpNode(enum BinopType _binop, std::unique_ptr<ExpNode> _left,
                 std::unique_ptr<ExpNode> _right):
        binop(_binop), left(std::move(_left)), right(std::move(_right)) {}

    void print_node(int indent) const override;
};

class UnopExpNode : public ExpNode {
public:
    enum UnopType {
        UNOP_NEG,
        UNOP_NOT,
        UNOP_DEREF,
        UNOP_ADDROF
    };

private:
    enum UnopType uop;
    std::unique_ptr<ExpNode> exp;

public:
    UnopExpNode(enum UnopType _uop, std::unique_ptr<ExpNode> _exp)
        : uop(_uop), exp(std::move(_exp)) {}

    void print_node(int indent) const override;
};

class FunCallExpNode : public ExpNode {
private:
    std::string func_name;
    std::vector<ExpNode*> func_args;

public:
    FunCallExpNode(const std::string& _func_name,
                    std::vector<ExpNode*>& _func_args)
        : func_name(_func_name), func_args(_func_args) {}

    ~FunCallExpNode() {
        for (auto& elem: func_args)
            delete elem;
    }

    void print_node(int indent) const override;
};

///===-------------------------------------------------------------------===///
/// Statements
///===-------------------------------------------------------------------===///

class StmtNode {
public:
    StmtNode() = default;
    virtual ~StmtNode() = default;
    virtual void print_stmt(int indent) const = 0;
};

class AssnStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> lhs, rhs;

public:
    AssnStmtNode(std::unique_ptr<ExpNode> _lhs, std::unique_ptr<ExpNode> _rhs)
        : lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

    void print_stmt(int indent) const override;
};

class DeclStmtNode : public StmtNode {
private:
    std::unique_ptr<Type> ty;
    std::string id;
    std::unique_ptr<ExpNode> exp;

public:
    DeclStmtNode(std::unique_ptr<Type> _ty,
                 const std::string& _id,
                 std::unique_ptr<ExpNode> _exp)
        : ty(std::move(_ty)), id(_id), exp(std::move(_exp)) {}

    void print_stmt(int indent) const override;
};

class SCallStmtNode : public StmtNode {
private:
    std::string fname;
    std::vector<ExpNode*> fargs;

public:
    SCallStmtNode(const std::string& _fname,
                    std::vector<ExpNode*>& _fargs)
        : fname(_fname), fargs(_fargs) {}

    ~SCallStmtNode() {
        for (auto& elem: fargs)
            delete elem;
    }

    void print_stmt(int indent) const override;
};

class RetStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> exp;

public:
    RetStmtNode(std::unique_ptr<ExpNode> _exp = nullptr)
        : exp(std::move(_exp)) {}

    void print_stmt(int indent) const override;
};

class IfStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<StmtNode*> then_body;
    std::vector<StmtNode*> else_body;

public:
    IfStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<StmtNode*>& _then_body,
                std::vector<StmtNode*>& _else_body)
        : cond(std::move(_cond)), then_body(_then_body),
            else_body(_else_body) {}

    ~IfStmtNode() {
        for (auto& elem: then_body)
            delete elem;

        for (auto& elem: else_body)
            delete elem;
    }

    void print_stmt(int indent) const override;
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
                std::vector<StmtNode*>& _body)
        : decl_list(_decl_list), cond(std::move(_cond)),
            iter(std::move(_iter)), body(_body) {}

    ~ForStmtNode() {
        for (auto& elem: decl_list)
            delete elem;

        for (auto& elem: body)
            delete elem;
    }

    void print_stmt(int indent) const override;
};

class WhileStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<StmtNode*> body;

public:
    WhileStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<StmtNode*>& _body)
        : cond(std::move(_cond)), body(_body) {}

    ~WhileStmtNode() {
        for (auto& elem: body)
            delete elem;
    }

    void print_stmt(int indent) const override;
};

///===-------------------------------------------------------------------===///
/// Top Level Declarations
///===-------------------------------------------------------------------===///
class Decls {
public:
    Decls() = default;
    virtual ~Decls() = default;
    virtual void print_decl(int indent) const = 0;
    virtual bool typecheck_decl() const = 0;
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
            std::vector<StmtNode*>& _block)
        : frtype(std::move(_frtype)), fname(_fname),
            args(_args), block(_block) {}

    ~FunDecl() {
        for (auto& elem: block)
            delete elem;

        for (auto& arg: args)
            delete arg.first;
    }

    void print_decl(int indent) const override;
};

class GlobalDecl : public Decls {
public:
    std::unique_ptr<Type> ty;
    std::string id;
    std::unique_ptr<ExpNode> exp;

    GlobalDecl(std::unique_ptr<Type> _ty,
               const std::string& _id,
               std::unique_ptr<ExpNode> _exp)
        : ty (std::move(_ty)), id (_id), exp(std::move(_exp)) {}

    void print_decl(int indent) const override;
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
    bool typecheck() const;
};

#endif // __AST_H__
