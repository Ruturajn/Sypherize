#include "../inc/llast.h"


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

void LLProg::print_ll_prog(std::ostream& os) const {
    int decls_sz = fdecls.size();
    for (int i = 0; i < decls_sz; i++) {
        fdecls[i].second->print_ll_fdecl(os, fdecls[i].first);
        if (i < decls_sz - 1)
            os << "\n\n";
    }
    os << "\n";
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
