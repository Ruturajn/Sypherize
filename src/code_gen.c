#include "../inc/code_gen.h"

void choose_target(TargetType target, ParsingContext *context,
                   AstNode *program_node) {
    if (context == NULL)
        print_error("NULL context encountered", 1, NULL);
    switch (target) {
    case TARGET_DEFAULT:
        break;
    case TARGET_X86_64_AT_T_ASM:
        target_x86_64_att_asm(context, program_node);
        break;
    default:
        break;
    }
}

void write_bytes(char *byte_str, FILE *fptr) {
    if (fptr == NULL)
        print_error(FILE_OPEN_ERR, 1, NULL);
    size_t len_bytes = strlen(byte_str);
    if (fwrite(byte_str, 1, len_bytes, fptr) != len_bytes)
        print_error("Code gen - Failed to write to file", 1, NULL);
}

void write_line(char *byte_str, FILE *fptr) {
    write_bytes(byte_str, fptr);
    write_bytes("\n", fptr);
}

void write_int(long int_val, FILE *fptr) {
    char byte_str[INT_STR_WIDTH] = {0};
    sprintf(byte_str, "%ld", int_val);
    write_bytes(byte_str, fptr);
}

void write_header(FILE *fptr) {
    write_bytes(";## ", fptr);
    write_bytes(SYPHERIZE_HEADER, fptr);
    write_line(" ##;", fptr);
}

void target_x86_64_att_asm_data_section(ParsingContext *context, FILE *fptr) {
    IdentifierBind *temp_bind = context->vars->binding;
    int status = -1;
    write_line(".section .data", fptr);
    while (temp_bind != NULL) {
        AstNode *var = get_env(context->env_type, temp_bind->id_val, &status);
        if (!status)
            print_error("Unable to retrieve value from environment", 1, NULL);

        write_bytes(temp_bind->identifier->ast_val.node_symbol, fptr);
        write_bytes(": .space ", fptr);
        write_int(var->child->ast_val.val, fptr);
        write_bytes("\n", fptr);

        temp_bind = temp_bind->next_id_bind;
    }
}

void target_x86_64_att_asm(ParsingContext *context, AstNode *program_node) {
    if (program_node == NULL || program_node->type != TYPE_PROGRAM)
        print_error("Error during code gen : Failed to access program node", 1,
                    NULL);

    FILE *fptr_code_gen = fopen("code_gen.S", "wb");
    CHECK_NULL(fptr_code_gen, "Error in creating code gen output file");

    AstNode *curr_expr = program_node->child;

    write_header(fptr_code_gen);
    target_x86_64_att_asm_data_section(context, fptr_code_gen);
    write_line(".section .text", fptr_code_gen);
    write_line(".global main", fptr_code_gen);
    write_line("main:", fptr_code_gen);
    // Save the base pointer by pushing it onto the stack
    write_line("push %rbp", fptr_code_gen);
    /**
     * Due to the above operation the stack pointer itself moves, since we added
     * the value of the base pointer to it. Hence, we set the base pointer to
     * the new location of the stack pointer.
     * | 1 | <-RBP (1) ; The base pointer is initially here.
     * | 1 |
     * | 1 | <-[RBP (1)] ;We push it onto the stack it moves. So SP also moves.
     * | 1 | <-RSP <- RBP ; Now, we set the base pointer to the SP.
     * | 1 |
     * | 1 |
     * | 1 |
     */
    write_line("mov %rsp, %rbp", fptr_code_gen);
    // We want to make some stack space just in case things mess up.
    // write_line("sub $32, %rsp", fptr_code_gen);

    while (curr_expr != NULL) {
        switch (curr_expr->type) {
        case TYPE_VAR_REASSIGNMENT:
            write_bytes("lea ", fptr_code_gen);
            write_bytes(curr_expr->child->ast_val.node_symbol, fptr_code_gen);
            write_line("(%rip), %rax", fptr_code_gen);
            write_bytes("movq $", fptr_code_gen);
            write_int(curr_expr->child->next_child->ast_val.val, fptr_code_gen);
            write_line(", (%rax)", fptr_code_gen);
            break;
        default:
            break;
        }
        curr_expr = curr_expr->next_child;
    }

    // write_line("add $32, %rsp", fptr_code_gen);
    write_line("pop %rbp", fptr_code_gen);
    // Move the value of rax register into rdi, which holds the exit code.
    write_line("movq (%rax), %rax", fptr_code_gen);
    write_line("ret", fptr_code_gen);

    /**
     * ; Custom C lib call in assembly
     * The first parameter for a C lib call on windows
     * gets stored in `%rcx`, and the second parameter gets
     * stored in `%rdx`.
     * - `fmt: .asciz "%ld"` above `.section .text`
     * - `movq $4303493, %rdx` ; Move a long integer into the 2nd arg.
     * - `lea fmt(%rip), %rcx ; Move the address of the string into the
     *                         ; firt argument.
     * - `call printf`
     */

    fclose(fptr_code_gen);
}
