#ifndef __TYPECHECKER_H__
#define __TYPECHECKER_H__

#include "./parser.h"
#include "./diagnostics.h"

using Environment =
    std::unordered_map<std::string, std::unordered_map<std::string, Type*>>;

// The first element in the vector is the function return type
using FuncEnvironment =
    std::unordered_map<std::string, std::vector<Type*>>;

class TypeChecker {
public:
    Environment env;
    FuncEnvironment fenv;
    Program* prog;

    TypeChecker(Program* _prog): env({}), fenv({}), prog(_prog) {};

    bool typecheck(Diagnostics* diag) {
        return prog->typecheck(env, fenv, diag);
    }
};

#endif // __TYPECHECKER_H__
