#ifndef __OPTIMIZE_H__
#define __OPTIMIZE_H__

#include "./llast.h"
#include <set>
#include <unordered_map>

class DfaCFG {
public:
    std::unordered_map<std::string, LLBlock*> blocks;
    std::unordered_map<std::string, std::set<std::string>> preds;
    LLType* ret_type;
    std::vector<std::pair<std::string, LLType*>> args;

    DfaCFG() : blocks({}), preds({}), ret_type(nullptr), args({}) {}

    void populate_cfg(const LLFDecl& llfdecl);

    ~DfaCFG() {
        /* for (auto& elem: blocks) */
        /*     delete elem.second; */

        delete ret_type;

        /* for (auto& elem: args) */
        /*     delete elem.second; */
    }
};

class Optimize {
public:
    std::vector<DfaCFG*> prog_dfa_cfg;

    Optimize(const LLProg& llprog) : prog_dfa_cfg({}) {
        for (auto& elem: llprog.fdecls) {
            auto dfa = new DfaCFG();
            dfa->populate_cfg(*elem.second);
            prog_dfa_cfg.push_back(dfa);
        }
    }

    ~Optimize() {
        for (auto& elem: prog_dfa_cfg)
            delete elem;
    }
};

#endif // __OPTIMIZE_H__
