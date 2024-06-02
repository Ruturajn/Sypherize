#include "../inc/llast.h"

///===-------------------------------------------------------------------===///
/// LLVM IR Ginits
///===-------------------------------------------------------------------===///

void LLGNull::print_ll_ginit(std::ostream& os) const {
    os << "null";
}

void LLGGid::print_ll_ginit(std::ostream& os) const {
    os << "@" << gid;
}

void LLGInt::print_ll_ginit(std::ostream& os) const {
    os << val;
}

void LLGString::print_ll_ginit(std::ostream& os) const {
    os << "c" << val;
}

void LLGArray::print_ll_ginit(std::ostream& os) const {
    os << "[ ";

    int ginit_sz = (int)ty_ginit_list.size();

    for (int i = 0; i < ginit_sz; i++) {
        ty_ginit_list[i].first->print_ll_type(os);
        os << " ";
        ty_ginit_list[i].second->print_ll_ginit(os);
        if (i < ginit_sz - 1)
            os << ", ";
    }

    os << "]";
}

void LLGStruct::print_ll_ginit(std::ostream& os) const {

    os << "{ ";

    int ginit_sz = (int)ty_ginit_list.size();

    for (int i = 0; i < ginit_sz; i++) {
        ty_ginit_list[i].first->print_ll_type(os);
        os << " ";
        ty_ginit_list[i].second->print_ll_ginit(os);
        if (i < ginit_sz - 1)
            os << ", ";
    }

    os << "}";
}

void LLGBitcast::print_ll_ginit(std::ostream& os) const {
    os << "bitcast (";
    from_ty->print_ll_type(os);
    os << " ";
    ginit->print_ll_ginit(os);
    os << " to ";
    to_ty->print_ll_type(os);
    os << ")";
}

///===-------------------------------------------------------------------===///
/// LLVM IR Decls
///===-------------------------------------------------------------------===///

void LLGDecl::print_ll_gdecl(std::ostream& os, const std::string& gid) const {
    os << "@" << gid << " = global ";
    ty->print_ll_type(os);
    os << " ";
    ginit->print_ll_ginit(os);
}

void LLCFG::print_ll_cfg(std::ostream& os) const {
    this->cfg.first->print_ll_block(os);

    for (auto b : cfg.second) {
        os << b.first << ":\n";
        b.second->print_ll_block(os);
    }
}

void LLBlock::print_ll_block(std::ostream& os) const {
    for (auto& insn : insn_list) {
        insn.second->print_ll_insn(os, insn.first);
        os << "\n";
    }

    term.second->print_ll_term(os);
    os << "\n";
}

void LLFDecl::print_ll_fdecl(std::ostream& os, const std::string& gid) const {

    os << "define ";
    func_ret_ty->print_ll_type(os);
    os << " @" << gid << "(";

    int arg_sz = (int)func_params_ty.size();
    for (int i = 0; i < arg_sz; i++) {
        func_params_ty[i]->print_ll_type(os);
        os << " %" << func_params[i];
        if (i < arg_sz - 1)
            os << ", ";
    }

    os << ") {\n";

    func_cfg->print_ll_cfg(os);

    os << "}";
}

///===-------------------------------------------------------------------===///
/// LLVM IR Program
///===-------------------------------------------------------------------===///

void LLProg::print_ll_prog(std::ostream& os) const {
    int gdecls_sz = gdecls.size();
    for (int i = 0; i < gdecls_sz; i++) {
        gdecls[i].second->print_ll_gdecl(os, gdecls[i].first);
        if (i < gdecls_sz - 1)
            os << "\n\n";
    }

    int decls_sz = fdecls.size();

    if (decls_sz == 0)
        os << "\n";
    else
        os << "\n\n";

    for (int i = 0; i < decls_sz; i++) {
        fdecls[i].second->print_ll_fdecl(os, fdecls[i].first);
        if (i < decls_sz - 1)
            os << "\n\n";
    }
    os << "\n";
}
