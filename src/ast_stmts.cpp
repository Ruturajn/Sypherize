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
