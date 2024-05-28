#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include "./ast.h"
#include "./llast.h"
#include <unordered_map>

class Frontend {
public:
    std::unordered_map<AST::IdExpNode, std::pair<LLType*, LLOperand*>> ctxt;
};

#endif // __FRONTEND_H__
