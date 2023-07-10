#include "../inc/ast_funcs.h"
#include "../inc/code_gen.h"
#include "../inc/helpers.h"
#include "../inc/lexer.h"
#include "../inc/parser.h"
#include "../inc/type_check.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_error(ERR_NUM_ARGS, "Invalid number of arguments : %d", NULL,
                    argc);
    }

    // Allocate the AST Node for the whole program.
    AstNode *program = node_alloc();
    program->type = TYPE_PROGRAM;

    ParsingContext *curr_context = create_default_parsing_context();

    // Open the file, lex and parse the file.
    lex_and_parse(argv[1], &curr_context, &program);

    print_parsing_context(curr_context, 0);

    // Type check the program.
    type_check_prog(curr_context, program);

    // Start Code generation.
    printf("\n[+]CODE GENERATION BEGIN...\n");
    target_codegen(curr_context, program, TARGET_x86_64_WIN);
    printf("[+]CODE GENERATION COMPLETE\n");

    free_node(program);

    exit(EXIT_SUCCESS);
}
