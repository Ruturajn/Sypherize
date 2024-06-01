#include "../inc/llast.h"

std::string gentemp_ll(const std::string& base) {
    static ssize_t cnt = 0;
    return "_" + base + std::to_string(cnt++);
}

///===-------------------------------------------------------------------===///
/// LLPtr
///===-------------------------------------------------------------------===///

bool LLTPtr::operator==(const LLType& other) const {
    if (typeid(*this) == typeid(other)) {
        const LLTPtr& llptr = static_cast<const LLTPtr&>(other);
        return static_cast<const LLType&>(*(*this).ty) == *(llptr.ty);
    }
    return false;
}

///===-------------------------------------------------------------------===///
/// LLTStruct
///===-------------------------------------------------------------------===///

void LLTStruct::print_ll_type(std::ostream& os) const {
    os << "{ ";

    int tsz = (int)ty_list.size();
    for (int i = 0; i < tsz; i++) {
        ty_list[i]->print_ll_type(os);
        if (i < tsz - 1)
            os << ", ";
    }

    os << "}";
}

bool LLTStruct::operator==(const LLType& other) const {
    if (typeid(*this) == typeid(other)) {
        const LLTStruct& llstruct = static_cast<const LLTStruct&>(other);
        for (int i = 0; i < (int)ty_list.size(); i++) {
            if (*ty_list[i] != *llstruct.ty_list[i])
                return false;
        }
        return true;
    }
    return false;
}

std::unique_ptr<LLType> LLTStruct::clone() const {
    std::vector<LLType*> clone_ty_list {};

    for (auto elem: ty_list)
        clone_ty_list.push_back(elem->clone().release());

    return std::make_unique<LLTStruct>(clone_ty_list);
}

///===-------------------------------------------------------------------===///
/// LLTArray
///===-------------------------------------------------------------------===///

void LLTArray::print_ll_type(std::ostream& os) const {
    /* os << "{ "; */

    /* os << "i64, "; */

    os << "[ " << arr_len << " x ";

    arr_ty->print_ll_type(os);

    os << " ] ";// } ";
}

bool LLTArray::operator==(const LLType& other) const {
    if (typeid(*this) == typeid(other)) {
        const LLTArray& llarr = static_cast<const LLTArray&>(other);
        if (this->arr_len != llarr.arr_len)
            return false;

        if (*this->arr_ty != *llarr.arr_ty)
            return false;

        return true;
    }
    return false;
}

///===-------------------------------------------------------------------===///
/// LLTFunc
///===-------------------------------------------------------------------===///

void LLTFunc::print_ll_type(std::ostream& os) const {
    os << "(";

    int arg_sz = (int)arg_ty_list.size();
    for (int i = 0; i < arg_sz; i++) {
        arg_ty_list[i]->print_ll_type(os);
        if (i < arg_sz - 1)
            os << ", ";
    }

    os << ") -> ";
    ret_ty->print_ll_type(os);
}

bool LLTFunc::operator==(const LLType& other) const {
    if (typeid(*this) == typeid(other)) {
        const LLTFunc& llfunc = static_cast<const LLTFunc&>(other);
        for (int i = 0; i < (int)arg_ty_list.size(); i++) {
            if (*arg_ty_list[i] != *llfunc.arg_ty_list[i])
                return false;
        }

        if (*ret_ty != *llfunc.ret_ty)
            return false;

        return true;
    }
    return false;
}
