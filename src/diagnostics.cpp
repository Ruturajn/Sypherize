#include "../inc/diagnostics.h"

bool Diagnostics::read_file(const char* program) {
    std::ifstream input_file;
    input_file.open(file_name);

    if (!input_file.is_open()) {
        std::cerr << "Invalid: file path!\n";
        std::cerr << "  Usage: " << program << " <FILE_PATH>\n";
        return false;
    }

    // Reserve space to avoid multiple reallocations (optional but efficient)
    input_file.seekg(0, std::ios::end);
    file_data.reserve(input_file.tellg());
    input_file.seekg(0, std::ios::beg);

    // Use std::copy to fill the string
    std::copy(std::istreambuf_iterator<char>(input_file),
              std::istreambuf_iterator<char>(),
              std::back_inserter(file_data));

    input_file.close();

    std::istringstream stream(file_data);
    std::string line;

    // Traverse each line using std::getline
    while (std::getline(stream, line)) {
        file_lines.push_back(line);
    }

    return true;
}

void Diagnostics::print_error(ssize_t l_n, ssize_t c_n, const char* error_str,
                               ssize_t highlight_len) {

    std::cerr << "\033[1;31m[ERR]:\033[1;37m " << error_str << "\n";
    std::cerr << ">> " << file_name << "[" << l_n << "," << c_n << "]\n";

    std::cerr << std::setw(8) << l_n << " | ";

    l_n -= 1;
    for (int i = 0; i < (int)file_lines[l_n].size(); i++) {
        if (i < c_n - 1 || i >= (c_n + highlight_len - 1))
            std::cerr << file_lines[l_n][i];
        else
            std::cerr << "\033[1;31m" << file_lines[l_n][i] << "\033[1;37m";
    }
    std::cerr << "\n";

    std::cerr << std::setw(8) << " " << " |";

    for (int i = 0; i < c_n; i++)
        std::cerr << ' ';

    std::cerr << "\033[1;31m";
    for (int i = 0; i < (int)highlight_len; i++)
        std::cerr << '^';

    std::cerr << "\033[1;37m\n\n";
}

void Diagnostics::print_error(const SRange& sr, const char* error_str) {
    if (sr.beg.l_num == sr.end.l_num)
        print_error(sr.beg.l_num, sr.beg.c_num, error_str,
            (sr.end.c_num - sr.beg.c_num) + 1);
    else {
        ssize_t line_sz = file_lines[sr.beg.l_num - 1].size();
        print_error(sr.beg.l_num, sr.beg.c_num, error_str, line_sz);
    }
}
