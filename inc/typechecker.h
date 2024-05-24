#ifndef __TYPECHECKER_H__
#define __TYPECHECKER_H__

#include "../inc/parser.h"

class TypeChecker {
    static const std::string global_key;
    const Program* prog;

    // This map binds function names to their local scope, which itself is a map
    // that binds variables used in the local scope to their type.
    // Global variables are available in all scopes, using the "__global__"
    // key, in the map.
    std::unordered_map<std::string,
        std::unordered_map<std::string, Type*>> env;

    TypeChecker(const Program* _prog) : prog(_prog), env({}) {}

    bool typecheck_prog();
};

#endif // __TYPECHECKER_H__
