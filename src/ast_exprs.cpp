#include "../inc/ast.h"

///===-------------------------------------------------------------------===///
/// NumberExpNode
///===-------------------------------------------------------------------===///

void NumberExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "NUM: " << val << "\n";
}

///===-------------------------------------------------------------------===///
/// StringExpNode
///===-------------------------------------------------------------------===///

void StringExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "STRING: " << val << "\n";
}

///===-------------------------------------------------------------------===///
/// BoolExpNode
///===-------------------------------------------------------------------===///

void BoolExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "BOOL: " << val << "\n";
}

///===-------------------------------------------------------------------===///
/// IdExpNode
///===-------------------------------------------------------------------===///

void IdExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";
    std::cout << "ID: " << val << "\n";
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

///===-------------------------------------------------------------------===///
/// BinopExpNode
///===-------------------------------------------------------------------===///

void BinopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

    std::cout << "BINOP: ";

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

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "LHS:\n";
    left->print_node(indent + 8);

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "RHS:\n";
    right->print_node(indent + 8);
}

///===-------------------------------------------------------------------===///
/// UnopExpNode
///===-------------------------------------------------------------------===///

void UnopExpNode::print_node(int indent) const {
    for (int i = 0; i < indent; i++)
        std::cout << " ";

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

    for (int i = 0; i < indent + 4; i++)
        std::cout << " ";

    std::cout << "EXP:\n";
    exp->print_node(indent + 8);
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
