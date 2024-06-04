#ifndef __CMD_OPS_H__
#define __CMD_OPS_H__

#include <ostream>
#include <unordered_map>
#include <vector>

enum class OptionType {
    OPTION_VERSION,
    OPTION_HELP,
    OPTION_VERBOSE,
    OPTION_INPUT,
    OPTION_OUTPUT,
    OPTION_PRINT_TOKENS,
    OPTION_PRINT_AST,
    OPTION_PRINT_LLVM,
    OPTION_ASM_DIALECT,
    OPTION_ASM,
    OPTION_RUN_LEXER,
    OPTION_RUN_PARSER,
    OPTION_RUN_TYPECHECKER,
    OPTION_RUN_COMPILE_TO_LLVM
};

struct CmdFlags {
    bool version;
    bool help;
    bool verbose;
    std::vector<std::string> input_files;
    std::string output_file;
    bool print_tokens;
    bool print_ast;
    bool print_llvm;
    enum AsmDialect {
        ASM_INTEL,
        ASM_ATT
    };
    AsmDialect asm_d;
    bool emit_asm;
    bool run_lexer;
    bool run_parser;
    bool run_typechecker;
    bool run_compile_to_llvm;

    CmdFlags() : version(false), help(false), verbose(false),
        input_files({}), output_file({}), print_tokens(false),
        print_ast(false), print_llvm(false), asm_d(ASM_ATT), emit_asm(false),
        run_lexer(false), run_parser(false), run_typechecker(false),
        run_compile_to_llvm(false) {}
};

class Option {
public:
    const char* op_long;    // Option long.
    const char* op_short;   // Option Short.
    const char* op_desc;    // Option description.
    std::vector<const char*> accepted_vals; // Accepted values for an option.
    bool is_required;   // Is the option required.
    OptionType op_ty;

    Option(const char* _op_long, const char* _op_short,
           const char* _op_desc, std::vector<const char*>& _accepted_vals,
           bool _is_required, OptionType _op_ty)
        : op_long(_op_long), op_short(_op_short), op_desc(_op_desc),
            accepted_vals(_accepted_vals), is_required(_is_required),
            op_ty(_op_ty) {}

    Option(const char* _op_long, const char* _op_short,
           const char* _op_desc, std::initializer_list<const char*> init_list,
           bool _is_required, OptionType _op_ty)
        : op_long(_op_long), op_short(_op_short), op_desc(_op_desc),
            accepted_vals(init_list), is_required(_is_required),
            op_ty(_op_ty) {}
};

class CmdOps {
public:
    // Maps passed option to its passed values.
    std::unordered_map<OptionType, std::vector<std::string>> passed_op_map;

    // Maps option string to its Option data structure.
    std::unordered_map<std::string, Option*> defined_op_map;

    // Takes in all the available options from the user.
    std::vector<Option> ops;

    // Output stream for dumping the output.
    std::ostream& os;

    const std::string files_key = "files";
    const char* prog_name = nullptr;

    CmdOps(std::vector<Option>& _ops, std::ostream& _os)
        : passed_op_map({}), defined_op_map({}), ops(_ops), os(_os) {}

    void print_error(const char* error_str) {
        os << "\033[1;31m[ERR]:\033[1;37m " << error_str << "\n\n";
        print_help_menu();
    }

    bool does_op_exist(Option& op) {
        return (passed_op_map.find(op.op_ty) != passed_op_map.end());
    }

    void get_op_val(std::vector<std::string>& res, Option& op) {
        if (does_op_exist(op))
            res.insert(res.end(),
                       passed_op_map[op.op_ty].begin(),
                       passed_op_map[op.op_ty].end());
    }

    void get_input_files(std::vector<std::string>& files) {
        files.insert(files.end(),
                     passed_op_map[OptionType::OPTION_INPUT].begin(),
                     passed_op_map[OptionType::OPTION_INPUT].end());
    }

    bool parse_cmd(int argc, char *argv[], int min_ops_required);
    void print_help_menu();
    void populate_cmd_flags(CmdFlags& cmd_flags);
};

#endif // __CMD_OPS_H__
