#include <fstream>
#include <iostream>
#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/typechecker.h"
#include "../inc/cmdops.h"

int main(int argc, char* argv[]) {

    std::vector<Option> options =
    {
        Option("--version", "-v", "Display current version of sypherc", {},
                false, OptionType::OPTION_VERSION),

        Option("--help", "-h", "Display help information for sypherc", {},
                false, OptionType::OPTION_HELP),

        Option("--verbose", "-V", "Display current version of sypherc", {},
                false, OptionType::OPTION_VERBOSE),

        Option("--input", "-i", "Path to input files", {},
                false, OptionType::OPTION_INPUT),

        Option("--output", "-o", "Path to the output file", {},
                false, OptionType::OPTION_OUTPUT),

        Option("--print-tokens", "-print-tokens",
                "Display the list of tokens produced by the lexer", {},
                false, OptionType::OPTION_PRINT_TOKENS),

        Option("--print-ast", "-print-ast", "Display the AST", {},
                false, OptionType::OPTION_PRINT_AST),

        Option("--llvm", "-l", "Compile to LLVM IR", {},
                false, OptionType::OPTION_LLVM),

        Option("--asm-dialect", "-asm-dialect",
                "Compile to specific assembly dialect", {"intel", "att"},
                false, OptionType::OPTION_ASM_DIALECT),

        Option("--asm", "-a", "Compile to assembly", {},
                false, OptionType::OPTION_ASM),
    };

    CmdOps cmd(options, std::cout);
    if (!cmd.parse_cmd(argc, argv, 2))
        return -1;

    CmdFlags cmd_flags;
    cmd.populate_cmd_flags(cmd_flags);

    Diagnostics diag(cmd_flags.input_files[0], &cmd);
    if (!diag.read_file())
        return -1;

    /// Lex
    Lexer lexer(diag.file_data);
    lexer.lex();
    if (lexer.failed)
        return -1;

    if (cmd_flags.verbose || cmd_flags.print_tokens)
        lexer.print_tokens();

    /// Parse
    Parser parser(lexer.tok_list, &diag);
    parser.parse_prog();
    if (parser.failed)
        return -1;

    if (cmd_flags.verbose || cmd_flags.print_ast)
        parser.prog.print_prog();

    /// Typecheck
    TypeChecker tc(&parser.prog);
    if (!tc.typecheck(&diag))
        return -1;

    /// Compile to LLVM

    return 0;
}
