#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include "./ast.h"
#include "./llast.h"
#include "diagnostics.h"
#include <unordered_map>

class Frontend {
public:
    std::unordered_map<std::string, std::pair<LLType*, LLOperand*>> ctxt;
    std::pair<std::pair<LLType*, std::unique_ptr<LLOperand>>, LLStream*> out;
    std::pair<LLGDecl*, std::vector<std::pair<std::string, LLGDecl*>>> gout;

    Diagnostics* diag;
    AST::Program* prog;
    LLProg llprog;

    Frontend(Diagnostics* _diag, AST::Program* _prog)
        : ctxt({}), out({}), gout({}), diag(_diag), prog(_prog), llprog({}) {
        ctxt["int"] = {new LLTi64, nullptr};
        ctxt["string"] = {new LLTPtr(std::make_unique<LLTi8>()), nullptr};
        ctxt["bool"] = {new LLTi1, nullptr};
    }

    ~Frontend() {
        delete ctxt["int"].first;
        delete ctxt["string"].first;
        delete ctxt["bool"].first;

        if (ctxt.find("null") != ctxt.end())
            delete ctxt["null"].first;

        delete out.second;

        for (auto& elem: ctxt) {
            if (elem.first == "int" || elem.first == "string" ||
                elem.first == "bool")
                continue;

            delete elem.second.first;
            delete elem.second.second;
        }
    }

    bool compile() {
        out.second = new LLStream();
        return prog->compile(ctxt, out, gout, diag, llprog);
    }

    void emit_llvm(std::ostream& os) {
        os << "; Generated From: " << diag->file_name << "\n";
        os << "target triple = \"x86_64-pc-linux-gnu\"\n\n";
        llprog.print_ll_prog(os);
    }
};

#endif // __FRONTEND_H__
