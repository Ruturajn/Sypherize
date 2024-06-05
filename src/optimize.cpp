#include "../inc/optimize.h"

void DfaCFG::populate_cfg(const LLFDecl& llfdecl) {
    std::set<std::string> tmp_set{};

    for (auto& [lbl, blk]: llfdecl.func_cfg->cfg) {
        blk->term.second->succs(tmp_set);
        for (auto& label: tmp_set)
            preds[label].insert(lbl);
        tmp_set.clear();

        blocks[lbl] = blk;
    }

    for (int i = 0; i < (int)llfdecl.func_params_ty.size(); i++)
        args.push_back({llfdecl.func_params[i], llfdecl.func_params_ty[i]});

    ret_type = llfdecl.func_ret_ty->clone().release();
}
