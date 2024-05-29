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

    Diagnostics* diag;
    AST::Program* prog;

    Frontend(Diagnostics* _diag, AST::Program* _prog)
        : ctxt({}), out({}), diag(_diag), prog(_prog) {
        const char* global_env = "__global__";
        ctxt["int"] = {new LLTi64, nullptr};
        ctxt["string"] = {new LLTPtr(std::make_unique<LLTi8>()), nullptr};
        ctxt["bool"] = {new LLTi1, nullptr};
    }

    ~Frontend() {
        // TODO: Make sure that the memory allocated for null type is freed.
        delete ctxt["int"].first;
        delete ctxt["string"].first;
        delete ctxt["bool"].first;
    }

    bool compile() {
        return prog->compile(ctxt, out, diag);
    }
};

#endif // __FRONTEND_H__
