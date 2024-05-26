#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

///===-------------------------------------------------------------------===///
/// Source Location
///===-------------------------------------------------------------------===///
class SLoc {
public:
    ssize_t l_num{};
    ssize_t c_num{};

    SLoc(ssize_t _l_num, ssize_t _c_num) : l_num(_l_num), c_num(_c_num) {}
    void print_sloc(std::ostream& os) const {
        os << "<" << l_num << "," << c_num << ">";
    }
};

class SRange {
public:
    SLoc beg;
    SLoc end;

    SRange(SLoc _beg, SLoc _end) : beg(_beg), end(_end) {}
    void print_srange(std::ostream& os) const {
        os << "[";
        beg.print_sloc(os);
        os << ",";
        end.print_sloc(os);
        os << "]";
    }
};


class Diagnostics {
public:
    std::string file_name;
    std::string file_data;
    std::vector<std::string> file_lines;

    Diagnostics(const std::string& _file_name)
        : file_name(_file_name), file_data({}), file_lines({}) {}

    bool read_file(const char* program);
    void print_error(ssize_t l_n, ssize_t c_n, const char* error_str,
                     ssize_t highlight_len);
    void print_error(const SRange& sr, const char* error_str);
};

#endif // __DIAGNOSTICS_H__
