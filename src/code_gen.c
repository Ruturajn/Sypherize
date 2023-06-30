#include "../inc/code_gen.h"

void choose_target(TargetType target, AstNode *program_node) {
    switch (target) {
    case TARGET_DEFAULT:
        break;
    case TARGET_X86_64_AT_T_ASM:
        target_x86_64_att_asm(program_node);
        break;
    default:
        break;
    }
}

void target_x86_64_att_asm(AstNode *program_node) {
    if (program_node == NULL || program_node->type != TYPE_PROGRAM)
        print_error("Error during code gen : Failed to access program node", 1,
                    NULL);

    FILE *fptr_code_gen = fopen("code_gen.txt", "wb");
    CHECK_NULL(fptr_code_gen, "Error in creating code gen output file");

    AstNode *curr_expr = program_node->child;

    while (curr_expr != NULL) {
        switch (curr_expr->type) {
        case TYPE_VAR_DECLARATION:
            // TODO
            break;
        default:
            break;
        }
        curr_expr = curr_expr->next_child;
    }

    fclose(fptr_code_gen);
}
