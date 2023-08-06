#include "../inc/ast_funcs.h"
#include "../inc/code_gen.h"
#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/type_check.h"
#include "../inc/utils.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("\nSee `%s --help`\n\n", argv[0]);
        print_error(ERR_ARGS, "Invalid number of arguments", NULL, 0);
    }

    int out_file_idx = -1;
    int in_file_idx = -1;
    enum TargetType out_fmt = TARGET_DEFAULT;
    int is_verbose = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("%s", USAGE_STRING);
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            i = i + 1;
            if (i > argc) {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS, "Expected Input file path after %s", argv[i - 1], 0);
            }
            if (*argv[i] == '-') {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS,
                            "Expected valid input file path got another "
                            "possible command line option : `%s`",
                            argv[i], 0);
            }
            in_file_idx = i;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            i = i + 1;
            if (i > argc) {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS, "Expected Output file path after %s", argv[i - 1], 0);
            }
            if (*argv[i] == '-') {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS,
                            "Expected valid file path got another possible "
                            "command line option : `%s`",
                            argv[i], 0);
            }
            out_file_idx = i;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
            i = i + 1;
            if (i > argc) {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS, "Expected Output format after : `%s`", argv[i - 1], 0);
            }
            if (*argv[i] == '-') {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS,
                            "Expected valid output format got another possible "
                            "command line option : `%s`",
                            argv[i], 0);
            }
            if (strcmp(argv[i], "default") == 0)
                out_fmt = TARGET_DEFAULT;
            else if (strcmp(argv[i], "x86_64-windows") == 0)
                out_fmt = TARGET_x86_64_WIN;
            else {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS, "Expected valid output format, got : `%s`", argv[i], 0);
            }
        } else if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--verbose") == 0) {
            is_verbose = 1;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("%s", VERSION_STRING);
            return 0;
        } else {
            if (*argv[i] == '-') {
                printf("\nSee `%s --help`\n\n", argv[0]);
                print_error(ERR_ARGS,
                            "Expected valid output format got another possible "
                            "command line option : `%s`",
                            argv[i], 0);
            }
            if (in_file_idx != -1)
                print_warning(ERR_ARGS, "Got multiple source files,uUsing latest : `%s`", argv[i],
                              0);
            in_file_idx = i;
        }
    }

    // Allocate the AST Node for the whole program.
    AstNode *program = node_alloc();
    program->type = TYPE_PROGRAM;

    ParsingContext *curr_context = create_default_parsing_context();

    // Open the file, lex and parse the file.
    if (in_file_idx == -1)
        print_error(ERR_ARGS, "Expected valid input file path", NULL, 0);
    lex_and_parse(argv[in_file_idx], &curr_context, &program);

    if (is_verbose == 1) {
        print_ast_node(program, 0);
        putchar('\n');
        print_parsing_context(curr_context, 0);
    }

    // Type check the program.
    type_check_prog(curr_context, program);

    // Start Code generation.
    if (is_verbose == 1)
        printf("\n[+]CODE GENERATION BEGIN...\n");
    char *file_name = (out_file_idx == -1) ? "code_gen.s" : argv[out_file_idx];
    target_codegen(curr_context, program, file_name, out_fmt);
    if (is_verbose == 1)
        printf("[+]CODE GENERATION COMPLETE\n");

    free_node(program);

    exit(EXIT_SUCCESS);
}
