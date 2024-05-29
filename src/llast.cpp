#include "../inc/llast.h"

std::string gentemp_ll(const std::string& base) {
    static ssize_t cnt = 0;
    return "_" + base + std::to_string(cnt++);
}
