#include "../inc/typechecker.h"

const std::string TypeChecker::global_key = "__global__";

bool TypeChecker::typecheck_prog() {
    return prog->typecheck();
}
