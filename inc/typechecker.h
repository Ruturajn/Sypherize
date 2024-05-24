#ifndef __TYPECHECKER_H__
#define __TYPECHECKER_H__

#include "../inc/parser.h"

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
};

#endif // __TYPECHECKER_H__
