#include "../inc/llast.h"

///===-------------------------------------------------------------------===///
/// LLVM IR Instructions
///===-------------------------------------------------------------------===///

void LLIBinop::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    switch (bop) {
        case LLBinopType::LLBINOP_ADD:
            os << "add";
            break;

        case LLBinopType::LLBINOP_SUB:
            os << "sub";
            break;

        case LLBinopType::LLBINOP_MULT:
            os << "mul";
            break;

        case LLBinopType::LLBINOP_DIVIDE:
            os << "sdiv";
            break;

        case LLBinopType::LLBINOP_REM:
            os << "srem";
            break;

        case LLBinopType::LLBINOP_LSHIFT:
            os << "shl";
            break;

        case LLBinopType::LLBINOP_RSHIFT:
            os << "ashr";
            break;

        case LLBinopType::LLBINOP_BITAND:
            os << "and";
            break;

        case LLBinopType::LLBINOP_BITOR:
            os << "or";
            break;

        case LLBinopType::LLBINOP_BITXOR:
            os << "xor";
            break;
    }

    os << " ";
    ty->print_ll_type(os);
    os << " ";
    op1->print_ll_op(os);
    os << ", ";
    op2->print_ll_op(os);
}

void LLIAlloca::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "alloca ";
    ty->print_ll_type(os);

    if (size_uid.size() != 0) {
        os << ", ";
        os << "%" << size_uid;
    }
}

void LLILoad::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "load ";
    ty->print_ll_type(os);
    os << ", ";

    ty->print_ll_type(os);
    os << "* ";
    op->print_ll_op(os);
}

void LLIStore::print_ll_insn(std::ostream& os, const std::string& uid) const {
    (void)uid;
    os << "\tstore ";
    ty->print_ll_type(os);
    os << " ";
    op_from->print_ll_op(os);
    os << ", ";

    ty->print_ll_type(os);
    os << "* ";
    op_to->print_ll_op(os);
}

void LLIIcmp::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "icmp ";

    switch (cond) {
        case LLCondType::LLCOND_EQUAL:
            os << "eq ";
            break;

        case LLCondType::LLCOND_NEQUAL:
            os << "neq ";
            break;

        case LLCondType::LLCOND_LT:
            os << "slt ";
            break;

        case LLCondType::LLCOND_LTE:
            os << "sle ";
            break;

        case LLCondType::LLCOND_GT:
            os << "sgt ";
            break;

        case LLCondType::LLCOND_GTE:
            os << "sge ";
            break;

    }

    ty->print_ll_type(os);
    os << " ";
    op1->print_ll_op(os);
    os << ", ";
    op2->print_ll_op(os);
}

void LLICall::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "call ";
    ty->print_ll_type(os);
    os << " ";
    op->print_ll_op(os);
    os << "(";

    int arg_sz = (int)ty_arg_list.size();

    for (int i = 0; i < arg_sz; i++) {
        ty_arg_list[i].first->print_ll_type(os);
        os << " ";

        ty_arg_list[i].second->print_ll_op(os);

        if (i < arg_sz - 1)
            os << ", ";
    }

    os << ")";
}

void LLIBitcast::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "bitcast ";
    from_ty->print_ll_type(os);
    os << " ";
    op->print_ll_op(os);
    os << " to ";
    to_ty->print_ll_type(os);
}

void LLIGep::print_ll_insn(std::ostream& os, const std::string& uid) const {
    os << "\t%" << uid << " = ";
    os << "getelementptr ";
    ty->print_ll_type(os);
    os << ", ";
    ty->print_ll_type(os);
    os << "* ";

    os << " ";
    op->print_ll_op(os);
    os << ", ";

    const LLOId lid("");

    int opsz = (int)op_list.size();
    for (int i = 0; i < opsz; i++) {
        auto op = op_list[i];
        if (typeid(*op) == typeid(lid))
            os << "i64 ";
        else
            os << "i32 ";
        op_list[i]->print_ll_op(os);

        if (i < opsz - 1)
            os << ", ";
    }
}

///===-------------------------------------------------------------------===///
/// LLVM IR Terminators
///===-------------------------------------------------------------------===///

void LLTermRet::print_ll_term(std::ostream& os) const {
    os << "\tret ";

    if (op == nullptr) {
        os << "void";
        return;
    }

    ty->print_ll_type(os);
    os << " ";
    op->print_ll_op(os);
}

void LLTermBr::print_ll_term(std::ostream& os) const {
    os << "\tbr label %" << lbl;
}

void LLTermCbr::print_ll_term(std::ostream& os) const {
    os << "\tbr i1 ";
    op->print_ll_op(os);

    os << ", label %" << true_lbl << ", label %" << false_lbl;
}

///===-------------------------------------------------------------------===///
/// LLVM IR Stream Elements
///===-------------------------------------------------------------------===///

void LLELables::print_ll_elt(std::ostream& os) const {
    os << lbl << ":\n";
}

void LLEInsn::print_ll_elt(std::ostream& os) const {
    insn->print_ll_insn(os, uid);
    os << "\n";
}

void LLETerm::print_ll_elt(std::ostream& os) const {
    term->print_ll_term(os);
    os << "\n";
}

void LLEGlbl::print_ll_elt(std::ostream& os) const {
    os << "UNIMPLEMENTED\n";
}
