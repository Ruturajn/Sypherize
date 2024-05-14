#ifndef __AST_H__
#define __AST_H__

#include <memory>
#include <vector>
#include <iostream>

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
    std::unique_ptr<Type> type;

public:
    TRef(std::unique_ptr<Type> _type) : type(std::move(_type)) {}
    void print_type() const override {
        std::cout << "[TRef]";
        type->print_type();
    }
};

///===-------------------------------------------------------------------===///
/// Expressions
///===-------------------------------------------------------------------===///

class ExpNode {
public:
    ExpNode() = default;
    virtual ~ExpNode() = default;
    virtual void print_node() const = 0;
    virtual std::unique_ptr<ExpNode> clone() = 0;
    virtual std::vector<std::unique_ptr<ExpNode>> get_children() = 0;
};

class NumberExpNode : public ExpNode {
private:
    long val;

public:
    NumberExpNode(long _val) : val(_val) {}

    void print_node() const override {
        std::cout << "NUM: " << val << "\n";
    }

    std::unique_ptr<ExpNode> clone() override {
        return std::make_unique<NumberExpNode>(*this);
    }

    std::vector<std::unique_ptr<ExpNode>> get_children() override {
        return {};
    }
};

class StringExpNode : public ExpNode {
private:
    std::string val;

public:
    StringExpNode(const std::string& _val) : val(_val) {}

    void print_node() const override {
        std::cout << "STRING: " << val << "\n";
    }

    std::unique_ptr<ExpNode> clone() override {
        return std::make_unique<StringExpNode>(*this);
    }

    std::vector<std::unique_ptr<ExpNode>> get_children() override {
        return {};
    }
};

class BoolExpNode : public ExpNode {
private:
    bool val;

public:
    BoolExpNode(bool _val) : val(_val) {}

    void print_node() const override {
        std::cout << "BOOL: " << val << "\n";
    }

    std::unique_ptr<ExpNode> clone() override {
        return std::make_unique<BoolExpNode>(*this);
    }

    std::vector<std::unique_ptr<ExpNode>> get_children() override {
        return {};
    }
};

class IdExpNode : public ExpNode {
private:
    std::string val;

public:
    IdExpNode(const std::string& _val) : val(_val) {}

    void print_node() const override {
        std::cout << "ID: " << val << "\n";
    }

    std::unique_ptr<ExpNode> clone() override {
        return std::make_unique<IdExpNode>(*this);
    }

    std::vector<std::unique_ptr<ExpNode>> get_children() override {
        return {};
    }
};

class CArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::vector<std::unique_ptr<ExpNode>> exp_list;

public:
    CArrExpNode(std::unique_ptr<Type> _ty,
                std::vector<std::unique_ptr<ExpNode>>& _exp_list)
        : ty(std::move(_ty)), exp_list(std::move(_exp_list)) {}

    void print_node() const override {
        std::cout << "CARR:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        ty->print_type();

        std::cout << " ";

        for (auto &e: exp_list) {
            e->print_node();
        }
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class NewArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::unique_ptr<ExpNode> exp;

public:
    NewArrExpNode(std::unique_ptr<Type> _ty,
                  std::unique_ptr<ExpNode> _exp)
        : ty (std::move(_ty)), exp(std::move(_exp)) {}

    void print_node() const override {
        std::cout << "NEWARR:\n";
        for (int i = 0; i < 4; i++)
            std::cout << " ";

        ty->print_type();
        std::cout << " ";

        exp->print_node();
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class IndexExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> exp;
    std::unique_ptr<ExpNode> idx;

public:
    IndexExpNode(std::unique_ptr<ExpNode> _exp,
                  std::unique_ptr<ExpNode> _idx)
        : exp(std::move(_exp)), idx(std::move(_idx)) {}

    void print_node() const override {
        std::cout << "INDEX:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        exp->print_node();
        idx->print_node();
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
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

    void print_node() const override {
        std::cout << "BINOP:\n";
        for (int i = 0; i < 4; i++)
            std::cout << " ";

        switch (binop) {
            case BINOP_PLUS:
                std::cout << "+";
                break;
            case BINOP_MINUS:
                std::cout << "-";
                break;
            case BINOP_MULT:
                std::cout << "*";
                break;
            case BINOP_DIVIDE:
                std::cout << "/";
                break;
            case BINOP_MODULUS:
                std::cout << "%";
                break;
            case BINOP_LSHIFT:
                std::cout << "<<";
                break;
            case BINOP_RSHIFT:
                std::cout << ">>";
                break;
            case BINOP_BITAND:
                std::cout << "&";
                break;
            case BINOP_BITOR:
                std::cout << "|";
                break;
            case BINOP_BITXOR:
                std::cout << "^";
                break;
            case BINOP_LT:
                std::cout << "<";
                break;
            case BINOP_LTE:
                std::cout << "<=";
                break;
            case BINOP_GT:
                std::cout << ">";
                break;
            case BINOP_GTE:
                std::cout << ">=";
                break;
            case BINOP_EQEQUAL:
                std::cout << "==";
                break;
            case BINOP_NEQUAL:
                std::cout << "!=";
                break;
            case BINOP_LOGAND:
                std::cout << "&&";
                break;
            case BINOP_LOGOR:
                std::cout << "||";
                break;
        }
        std::cout << "\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "LHS:\n";
        left->print_node();

        std::cout << "LHS:\n";
        right->print_node();
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
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

    void print_node() const override {
        std::cout << "UNOP:\n";
        switch (uop) {
            case UNOP_NEG:
                std::cout << "~";
                break;
            case UNOP_NOT:
                std::cout << "!";
                break;
            case UNOP_DEREF:
                std::cout << "@";
                break;
            case UNOP_ADDROF:
                std::cout << "#";
                break;
        }
        std::cout << "\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "EXP:\n";
        exp->print_node();
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class FunCallExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> func_name;
    std::vector<std::unique_ptr<ExpNode>> func_args;

public:
    FunCallExpNode(std::unique_ptr<ExpNode> _func_name,
                    std::vector<std::unique_ptr<ExpNode>>& _func_args)
        : func_name(std::move(_func_name)), func_args(_func_args) {}

    void print_node() const override {
        std::cout << "FUNCALL:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "FUNNAME: ";

        func_name->print_node();

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "FUNARGS: ";

        for (auto &arg: func_args)
            arg->print_node();
    }

    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

///===-------------------------------------------------------------------===///
/// Statements
///===-------------------------------------------------------------------===///

class StmtNode {
public:
    StmtNode() = default;
    virtual ~StmtNode() = default;
    virtual void print_stmt() const = 0;
    virtual std::unique_ptr<StmtNode> clone() = 0;
    virtual std::vector<std::unique_ptr<ExpNode>> get_children() = 0;
};

class AssnStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> lhs, rhs;

public:
    AssnStmtNode(std::unique_ptr<ExpNode> _lhs, std::unique_ptr<ExpNode> _rhs)
        : lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}

    void print_stmt() const override {
        std::cout << "ASSN:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "LHS:\n";
        lhs->print_node();

        std::cout << "RHS:\n";
        rhs->print_node();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class DeclStmtNode : public StmtNode {
private:
    std::string id;
    std::unique_ptr<ExpNode> exp;

public:
    DeclStmtNode(const std::string& _id, std::unique_ptr<ExpNode> _exp)
        : id(_id), exp(std::move(_exp)) {}

    void print_stmt() const override {
        std::cout << "DECL:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "ID: " << id << "\n";

        std::cout << "EXP:\n";
        exp->print_node();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class SCallStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> fname;
    std::vector<std::unique_ptr<ExpNode>> fargs;

public:
    SCallStmtNode(std::unique_ptr<ExpNode> _fname,
                    std::vector<std::unique_ptr<ExpNode>>& _fargs)
        : fname(std::move(_fname)), fargs(_fargs) {}

    void print_stmt() const override {
        std::cout << "SFUNCALL:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "FUNNAME: ";

        fname->print_node();

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "FUNARGS: ";

        for (auto &arg: fargs)
            arg->print_node();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class RetStmtNode : public StmtNode {
private:
    std::unique_ptr<Type> ty;
    std::unique_ptr<ExpNode> exp;

public:
    RetStmtNode(std::unique_ptr<Type> _ty, std::unique_ptr<ExpNode> _exp)
        : ty(std::move(_ty)), exp(std::move(_exp)) {}

    void print_stmt() const override {
        std::cout << "RETURN:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        exp->print_node();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class IfStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<std::unique_ptr<StmtNode>> then_body;
    std::vector<std::unique_ptr<StmtNode>> else_body;

public:
    IfStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<std::unique_ptr<StmtNode>>& _then_body,
                std::vector<std::unique_ptr<StmtNode>>& _else_body)
        : cond(std::move(_cond)), then_body(_then_body),
            else_body(_else_body) {}

    void print_stmt() const override {
        std::cout << "IF:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "COND:\n";
        cond->print_node();

        std::cout << "THEN_BODY:\n";
        for (auto &s: then_body)
            s->print_stmt();

        std::cout << "ELSE_BODY:\n";
        for (auto &s: else_body)
            s->print_stmt();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class ForStmtNode : public StmtNode {
private:
    std::vector<std::unique_ptr<ExpNode>> decl_list;
    std::unique_ptr<ExpNode> cond;
    std::unique_ptr<StmtNode> iter;
    std::vector<std::unique_ptr<StmtNode>> body;

public:
    ForStmtNode(std::vector<std::unique_ptr<ExpNode>>& _decl_list,
                std::unique_ptr<ExpNode> _cond,
                std::unique_ptr<StmtNode> _iter,
                std::vector<std::unique_ptr<StmtNode>>& _body)
        : decl_list(_decl_list), cond(std::move(_cond)),
            iter(std::move(_iter)), body(_body) {}

    void print_stmt() const override {
        std::cout << "FOR:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "DECL_LIST:\n";
        for (auto &d: decl_list)
            d->print_node();

        std::cout << "COND:\n";
        cond->print_node();

        std::cout << "ITER:\n";
        iter->print_stmt();

        std::cout << "BODY:\n";
        for (auto &b: body)
            b->print_stmt();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class WhileStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<std::unique_ptr<StmtNode>> body;

public:
    WhileStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<std::unique_ptr<StmtNode>>& _body)
        : cond(std::move(_cond)), body(_body) {}

    void print_stmt() const override {
        std::cout << "WHILE:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "COND:\n";
        cond->print_node();

        std::cout << "BODY:\n";
        for (auto &b: body)
            b->print_stmt();
    }

    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

///===-------------------------------------------------------------------===///
/// Top Level Declarations
///===-------------------------------------------------------------------===///
class Decls {
public:
    Decls() = default;
    virtual ~Decls() = default;
    virtual void print_decl() const = 0;
};

class FunDecl : public Decls {
public:
    std::unique_ptr<Type> frtype;
    std::string fname;
    std::vector<std::pair<std::unique_ptr<Type>, std::string>> args;
    std::vector<std::unique_ptr<StmtNode>> block;

    FunDecl(std::unique_ptr<Type> _frtype,
            const std::string& _fname,
            std::vector<std::pair<std::unique_ptr<Type>, std::string>>& _args,
            std::vector<std::unique_ptr<StmtNode>>& _block)
        : frtype(std::move(_frtype)), fname(_fname),
            args(_args), block(_block) {}

    void print_decl() const override {
        std::cout << "FUN_DECL:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "FUNNAME: " << fname << "\n";

        std::cout << "FUNARGS: ";

        for (auto &arg: args) {
            std::cout << "ARG:";
            arg.first->print_type();
            std::cout << arg.second << "\n";
        }

        std::cout << "FUNBODY:\n";

        for (auto &s: block)
            s->print_stmt();
    }
};

class GlobalDecl : public Decls {
public:
    std::string id;
    std::unique_ptr<ExpNode> exp;

    GlobalDecl(const std::string& _id,
               std::unique_ptr<ExpNode> _exp)
        : id (_id), exp(std::move(_exp)) {}

    void print_decl() const override {
        std::cout << "GLOBAL_DECL:\n";

        for (int i = 0; i < 4; i++)
            std::cout << " ";

        std::cout << "ID: " << id << "\n";
        std::cout << "EXP:\n";
        exp->print_node();
    }
};

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///
class Program {
public:
    std::vector<std::unique_ptr<Decls>> decl_list;

    Program(std::vector<std::unique_ptr<Decls>>& _decl_list)
        : decl_list(_decl_list) {}

    void print_prog() const {
        std::cout << "PROG:\n";

        for (auto &d: decl_list)
            d->print_decl();
    }
};

#endif // __AST_H__
