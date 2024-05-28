#include "../inc/cmdops.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ostream>

bool CmdOps::parse_cmd(int argc, char* argv[], int min_ops_required) {

    for (auto& op: ops) {
        if (defined_op_map.find(op.op_long) != defined_op_map.end())
            print_error("Duplicate long option name encountered");

        defined_op_map[op.op_long] = &op;

        if (defined_op_map.find(op.op_short) != defined_op_map.end())
            print_error("Duplicate short option name encountered");

        defined_op_map[op.op_short] = &op;
    }

    prog_name = *argv++;

    if (argc < min_ops_required) {
        print_error("Too few arguments provided");
        return false;
    }

    const char* op = nullptr;
    const char* val = nullptr;

    while (*argv != nullptr) {
        op = *argv++;

        if (defined_op_map.find(op) == defined_op_map.end()) {
            if (passed_op_map.find(OptionType::OPTION_INPUT) == passed_op_map.end())
                passed_op_map[OptionType::OPTION_INPUT] = {};

            passed_op_map[OptionType::OPTION_INPUT].push_back(op);

            continue;

            //TODO: parse flags like this `-lnc`
        }

        auto op_ptr = defined_op_map[op];
        auto& acc_vals = op_ptr->accepted_vals;

        if (acc_vals.size() == 0) {
            passed_op_map[op_ptr->op_ty] = {};
            continue;
        }

        val = *argv++;

        auto it = std::find(acc_vals.begin(), acc_vals.end(), val);

        if (it == acc_vals.end()) {
            std::string err = "Invalid value: " + std::string(val)
                + " for option - " + std::string(op);
            print_error(err.c_str());
            return false;
        }

        if (passed_op_map.find(op_ptr->op_ty) == passed_op_map.end())
            passed_op_map[op_ptr->op_ty] = {};

        passed_op_map[op_ptr->op_ty].push_back(val);
    }

    if (passed_op_map.find(OptionType::OPTION_INPUT) == passed_op_map.end()) {
        print_error("No input files provided");
        return false;
    }

    for (auto& op: ops) {
        if (op.is_required && passed_op_map.find(op.op_ty) == passed_op_map.end()) {
            std::string err = "Required option: " + std::string(op.op_long) + "/" +
                std::string(op.op_short) + " not passed";
            print_error(err.c_str());
            return false;
        }
    }

    return true;
}

void CmdOps::populate_cmd_flags(CmdFlags& cmd_flags) {
    for (auto& op: ops) {
        if (!does_op_exist(op))
            continue;

        switch (op.op_ty) {
            case OptionType::OPTION_VERSION:
                cmd_flags.version = true;
                break;

            case OptionType::OPTION_HELP:
                cmd_flags.help = true;
                break;

            case OptionType::OPTION_VERBOSE:
                cmd_flags.verbose = true;
                break;

            case OptionType::OPTION_INPUT:
                get_input_files(cmd_flags.input_files);
                break;

            case OptionType::OPTION_OUTPUT:
                cmd_flags.output_file = passed_op_map[op.op_ty][0];
                break;

            case OptionType::OPTION_PRINT_TOKENS:
                cmd_flags.print_tokens = true;
                break;

            case OptionType::OPTION_PRINT_AST:
                cmd_flags.print_ast = true;
                break;

            case OptionType::OPTION_LLVM:
                cmd_flags.emit_llvm = true;
                break;

            case OptionType::OPTION_ASM_DIALECT: {
                int asm_d = 0;
                for (int i = 0; i < (int)op.accepted_vals.size(); i++) {
                    if (op.accepted_vals[i] == passed_op_map[op.op_ty][0]) {
                        asm_d = i;
                        break;
                    }
                }
                cmd_flags.asm_d = static_cast<CmdFlags::AsmDialect>(asm_d);
                break;
            }

            case OptionType::OPTION_ASM:
                cmd_flags.emit_asm = true;
                break;

            case OptionType::OPTION_RUN_LEXER:
                cmd_flags.run_lexer = true;
                break;

            case OptionType::OPTION_RUN_PARSER:
                cmd_flags.run_parser = true;
                break;

            case OptionType::OPTION_RUN_TYPECHECKER:
                cmd_flags.run_typechecker = true;
                break;
        }
    }
}

void CmdOps::print_help_menu() {
    os << "\033[0;33mUSAGE:\033[1;37m\n";
    os << std::string(4, ' ') << prog_name << " [OPTIONS] FILE...\n\n";

    os << "\033[0;33mOPTIONS:\033[1;37m\n";
    for (auto& op: ops) {
        os << std::left << std::setw(15) << "\033[0;32m" + std::string(4, ' ') +
            std::string(op.op_short) + ", " + std::string(op.op_long) + "\033[1;37m";

        int sz = (int)op.accepted_vals.size();
        if (sz != 0) {
            os << " \033[0;32m[";
            for (int i = 0; i < sz; i++) {
                os << op.accepted_vals[i];
                if (i != sz - 1)
                    os << "|";
            }
            os << "]\033[1;37m";
        }
        os << "\n";

        os << "\t" << op.op_desc << "\n\n";
    }
}
