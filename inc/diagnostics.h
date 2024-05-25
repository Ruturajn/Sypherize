#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

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
};

#endif // __DIAGNOSTICS_H__
