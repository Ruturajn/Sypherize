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

    TypeChecker(Program* _prog): env({}), fenv({}), prog(_prog) {
        const char* global_env = "__global__";
        env[global_env]["int"] = new TInt;
        env[global_env]["string"] = new TString;
        env[global_env]["bool"] = new TBool;
    }

    ~TypeChecker() {
        delete env["__global__"]["int"];
        delete env["__global__"]["string"];
        delete env["__global__"]["bool"];

        if (env["__global__"].find("__addrof__") != env["__global__"].end())
            delete env["__global__"]["__addrof__"];
    }

    bool typecheck(Diagnostics* diag) {
        return prog->typecheck(env, fenv, diag);
    }
};

#endif // __TYPECHECKER_H__
