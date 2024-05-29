#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include "./ast.h"
#include "./llast.h"
#include "diagnostics.h"
#include <unordered_map>

class Frontend {
public:
    std::unordered_map<AST::IdExpNode*, std::pair<LLType*, LLOperand*>> ctxt;
    std::pair<std::pair<LLType*, LLOperand*>, LLStream*> out;
    Diagnostics* diag;
    AST::Program* prog;

    Frontend(Diagnostics* _diag, AST::Program* _prog)
        : ctxt({}), out({}), diag(_diag), prog(_prog) {}

    bool compile() {
        return prog->compile(ctxt, out, diag);
    }
};

#endif // __FRONTEND_H__
