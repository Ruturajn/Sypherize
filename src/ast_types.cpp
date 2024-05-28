#include "../inc/ast.h"

using namespace AST;

///===-------------------------------------------------------------------===///
/// TInt
///===-------------------------------------------------------------------===///

bool TInt::is_valid_unop(UnopType u) const {
    switch (u) {
        case UnopType::UNOP_NEG:
        case UnopType::UNOP_NOT:
        case UnopType::UNOP_ADDROF:
            return true;

        default:
            return false;
    }
}

///===-------------------------------------------------------------------===///
/// TBool
///===-------------------------------------------------------------------===///

bool TBool::is_valid_binop(BinopType b) const {
    switch (b) {
        case BinopType::BINOP_EQEQUAL:
        case BinopType::BINOP_NEQUAL:
        case BinopType::BINOP_LOGAND:
        case BinopType::BINOP_LOGOR:
            return true;
        default:
            return false;
    }
}
bool TBool::is_valid_unop(UnopType u) const {
    switch (u) {
        case UnopType::UNOP_NOT:
            return true;

        default:
            return false;
    }
}

///===-------------------------------------------------------------------===///
/// TString
///===-------------------------------------------------------------------===///

bool TString::is_valid_binop(BinopType b) const {
    switch (b) {
        case BinopType::BINOP_PLUS:
        case BinopType::BINOP_MULT:
        case BinopType::BINOP_EQEQUAL:
        case BinopType::BINOP_NEQUAL:
            return true;

        default:
            return false;
    }
}

///===-------------------------------------------------------------------===///
/// TRef
///===-------------------------------------------------------------------===///

std::string TRef::get_source_type() const {
    return type->get_source_type() + "@";
}

bool TRef::operator==(const Type& other) const {
    if (typeid(*this) == typeid(other)) {
        const TRef& tref = static_cast<const TRef&>(other);
        return static_cast<const Type&>(*(*this).type) == *(tref.type);
    }
    return false;
}

bool TRef::is_valid_binop(BinopType b) const {
    switch (b) {
        case BinopType::BINOP_PLUS:
        case BinopType::BINOP_EQEQUAL:
        case BinopType::BINOP_NEQUAL:
            return true;

        default:
            return false;
    }
}

bool TRef::is_valid_unop(UnopType u) const {
    switch (u) {
        case UnopType::UNOP_ADDROF:
        case UnopType::UNOP_DEREF:
            return true;

        default:
            return false;
    }
}

///===-------------------------------------------------------------------===///
/// TArray
///===-------------------------------------------------------------------===///

std::string TArray::get_source_type() const {
    return type->get_source_type() + "[]";
}

bool TArray::operator==(const Type& other) const {
    if (typeid(*this) == typeid(other)) {
        const TArray& tarr = static_cast<const TArray&>(other);
        return static_cast<const Type&>(*(*this).type) == *(tarr.type);
    }
    return false;
}

bool TArray::is_valid_unop(UnopType u) const {
    switch (u) {
        case UnopType::UNOP_ADDROF:
            return true;

        default:
            return false;
    }
}
