#include <memory>
#include <vector>

///===-------------------------------------------------------------------===///
/// Types
///===-------------------------------------------------------------------===///
class Type {
public:
    Type() = default;
    virtual ~Type() = default;
};

class TInt : public Type {};

class TBool : public Type {};

class TString : public Type {};

class TVoid : public Type {};

class TRef : public Type {
private:
    std::unique_ptr<Type> type;

public:
    TRef(std::unique_ptr<Type> _type) : type(std::move(_type)) {}
};

///===-------------------------------------------------------------------===///
/// Expressions
///===-------------------------------------------------------------------===///

class ExpNode {
public:
    ExpNode() = delete;
    virtual ~ExpNode() = default;
    virtual void print_node() const = 0;
    virtual std::unique_ptr<ExpNode> clone() = 0;
    virtual std::vector<std::unique_ptr<ExpNode>> get_children() = 0;
};

class NumberExpNode : public ExpNode {
private:
    long val;

public:
    NumberExpNode(long _val);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class StringExpNode : public ExpNode {
private:
    std::string val;

public:
    StringExpNode(const std::string& _val);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class BoolExpNode : public ExpNode {
private:
    bool val;

public:
    BoolExpNode(bool _val);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class IdExpNode : public ExpNode {
private:
    std::string val;

public:
    IdExpNode(const std::string& _val);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class CArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::vector<std::unique_ptr<ExpNode>> exp_list;

public:
    CArrExpNode(std::unique_ptr<Type> _ty,
                std::vector<std::unique_ptr<ExpNode>>& _exp_list);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class NewArrExpNode : public ExpNode {
private:
    std::unique_ptr<Type> ty;
    std::unique_ptr<ExpNode> exp;

public:
    NewArrExpNode(std::unique_ptr<Type> _ty,
                  std::unique_ptr<ExpNode> _exp);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class IndexExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> exp;
    std::unique_ptr<ExpNode> idx;

public:
    IndexExpNode(std::unique_ptr<ExpNode> _exp,
                  std::unique_ptr<ExpNode> _idx);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class BinopExpNode : public ExpNode {
private:
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
        BINOP_BITXOR
    };
    enum BinopType binop;
    std::unique_ptr<ExpNode> left, right;

public:
    BinopExpNode(enum BinopType _binop, std::unique_ptr<ExpNode> _left,
                 std::unique_ptr<ExpNode> _right);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class UnopExpNode : public ExpNode {
private:
    enum UnopType {
        UNOP_NEG,
        UNOP_NOT,
        UNOP_DEREF,
        UNOP_ADDROF
    };
    enum UnopType uop;
    std::unique_ptr<ExpNode> exp;

public:
    UnopExpNode(enum UnopType _uop, std::unique_ptr<ExpNode> _left,
                 std::unique_ptr<ExpNode> _right);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class FunCallExpNode : public ExpNode {
private:
    std::unique_ptr<ExpNode> func_name;
    std::vector<std::unique_ptr<ExpNode>> func_args;

public:
    FunCallExpNode(std::unique_ptr<ExpNode> _func_name,
                    std::vector<std::unique_ptr<ExpNode>>& _func_args);
    void print_node() const override;
    std::unique_ptr<ExpNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

///===-------------------------------------------------------------------===///
/// Statements
///===-------------------------------------------------------------------===///

class StmtNode {
public:
    StmtNode() = delete;
    virtual ~StmtNode() = default;
    virtual void print_stmt() const = 0;
    virtual std::unique_ptr<StmtNode> clone() = 0;
    virtual std::vector<std::unique_ptr<ExpNode>> get_children() = 0;
};

class AssnStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> lhs, rhs;

public:
    AssnStmtNode(std::unique_ptr<ExpNode> _lhs, std::unique_ptr<ExpNode> _rhs);
    void print_stmt() const override;
    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class DeclStmtNode : public StmtNode {
private:
    std::string id;
    std::unique_ptr<ExpNode> exp;

public:
    DeclStmtNode(const std::string& _id, std::unique_ptr<ExpNode> _exp);
    void print_stmt() const override;
    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class SCallStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> fname;
    std::unique_ptr<ExpNode> fargs;

public:
    SCallStmtNode(std::unique_ptr<ExpNode> _fname,
                    std::unique_ptr<ExpNode> _fargs);
    void print_stmt() const override;
    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class RetStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> exp;

public:
    RetStmtNode(std::unique_ptr<ExpNode> _exp);
    void print_stmt() const override;
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
                std::vector<std::unique_ptr<StmtNode>>& _else_body);
    void print_stmt() const override;
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
                std::vector<std::unique_ptr<StmtNode>>& _body);
    void print_stmt() const override;
    std::unique_ptr<StmtNode> clone() override;
    std::vector<std::unique_ptr<ExpNode>> get_children() override;
};

class WhileStmtNode : public StmtNode {
private:
    std::unique_ptr<ExpNode> cond;
    std::vector<std::unique_ptr<StmtNode>> body;

public:
    WhileStmtNode(std::unique_ptr<ExpNode> _cond,
                std::vector<std::unique_ptr<StmtNode>>& _body);
    void print_stmt() const override;
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
            std::vector<std::unique_ptr<StmtNode>>& _block);
};

class GlobalDecl : public Decls {
public:
    std::string id;
    std::unique_ptr<ExpNode> exp;

    GlobalDecl(const std::string& _id,
               std::unique_ptr<ExpNode> _exp);
};

///===-------------------------------------------------------------------===///
/// Program
///===-------------------------------------------------------------------===///
class Program {
public:
    std::vector<std::unique_ptr<Decls>> decl_list;

    Program(std::vector<std::unique_ptr<Decls>>& _decl_list);
};
