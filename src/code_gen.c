#include "../inc/code_gen.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/parser.h"
#include "../inc/utils.h"
#include <inttypes.h>

char label_arr[LABEL_ARR_SIZE];
int label_cnt = 0;
int label_idx = 0;

char sym_arr[SYM_ARR_SIZE];
int sym_cnt = 0;
int sym_idx = 0;

char codegen_verbose = 1;

const char *comp_suffixes_x86_84[COMP_COUNT] = {
    "e", "ne", "l", "le", "g", "ge",
};

char is_valid_reg_desc(CGContext *cg_ctx, RegDescriptor reg_desc) {
    return reg_desc >= 0 && reg_desc <= cg_ctx->reg_pool.reg_cnt;
}

RegDescriptor reg_alloc(CGContext *cg_ctx) {
    if (!(cg_ctx->reg_pool.reg_cnt > 0 && cg_ctx->reg_pool.scratch_reg_cnt > 0))
        print_error(ERR_COMMON, "Found empty register pool");

    // Iterate through the register pool to find un-used register.
    Reg *reg_iterator = *cg_ctx->reg_pool.scratch_regs;
    for (RegDescriptor i = 0; i < cg_ctx->reg_pool.scratch_reg_cnt; i++) {
        if (reg_iterator[i].reg_in_use == 0) {
            reg_iterator[i].reg_in_use = 1;
            return reg_iterator[i].reg_desc;
        }
    }

    print_error(ERR_MEM, "Unable to allocate a new register");
    return -1;
}

void reg_dealloc(CGContext *cg_ctx, RegDescriptor reg_desc) {
    if (!is_valid_reg_desc(cg_ctx, reg_desc))
        print_error(ERR_COMMON, "Encountered invalid register descriptor");

    Reg *temp = cg_ctx->reg_pool.regs;
    temp[reg_desc].reg_in_use = 0;
}

// Generate labels for lambda functions.
static char *gen_label() {
    char *label = label_arr + label_idx;
    label_idx += snprintf(label, LABEL_ARR_SIZE - label_idx, ".L%d", label_cnt);
    label_idx++;
    if (label_idx > LABEL_ARR_SIZE) {
        label_idx = 0;
        return gen_label();
    }
    label_cnt++;
    return label;
}

typedef struct SymToAddr {
    enum {
        SYM_ADDR_GLOBAL,
        SYM_ADDR_LOCAL,
    } type;
    union {
        const char *global;
        long local;
    } val;

} SymToAddr;

SymToAddr map_sym_to_addr_win(CGContext *cg_ctx, AstNode *sym_node) {
    if (sym_node == NULL || sym_node->ast_val.node_symbol == NULL ||
        cg_ctx == NULL)
        print_error(ERR_DEV,
                    "map_sym_to_addr_win() : NULL symbol/codegen context");

    SymToAddr sym_addr;

    if (cg_ctx->parent_ctx == NULL) {
        sym_addr.type = SYM_ADDR_GLOBAL;
        sym_addr.val.global = sym_node->ast_val.node_symbol;
        return sym_addr;
    }

    int stat = -1;
    AstNode *local_var = get_env(cg_ctx->local_env, sym_node, &stat);
    if (stat == 0)
        print_error(ERR_COMMON,
                    "Unable to find information in code gen "
                    "context for : `%s`",
                    sym_node->ast_val.node_symbol);

    long addr = local_var->ast_val.val;
    free(local_var);
    sym_addr.type = SYM_ADDR_LOCAL;
    sym_addr.val.local = addr;

    return sym_addr;
}

void target_x86_64_win_codegen_comp(CGContext *cg_ctx, AstNode *curr_expr,
                                    enum ComparisonType comp_type,
                                    FILE *fptr_code) {
    if (comp_type > COMP_COUNT)
        print_error(ERR_COMMON,
                    "Encountered invalid comparison during code gen");

    curr_expr->result_reg_desc = reg_alloc(cg_ctx);

    // Zero out result register.
    fprintf(fptr_code, "xor %s, %s\n",
            get_reg_name(cg_ctx, curr_expr->result_reg_desc),
            get_reg_name(cg_ctx, curr_expr->result_reg_desc));

    // Subtracts second operand from the first operand and sets the EFLAGS
    // ,i.e., OF, CF, etc. The set<op> then checks those flags for specific
    // condition.
    fprintf(fptr_code, "cmp %s, %s\n",
            get_reg_name(cg_ctx, curr_expr->child->next_child->result_reg_desc),
            get_reg_name(cg_ctx, curr_expr->child->result_reg_desc));
    fprintf(
        fptr_code, "set%s %s\n", comp_suffixes_x86_84[comp_type],
        get_byte_reg_name_x86_64_gnu_as(cg_ctx, curr_expr->result_reg_desc));

    // De-alloc LHS and RHS registers.
    reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
    reg_dealloc(cg_ctx, curr_expr->child->next_child->result_reg_desc);
}

void target_x86_64_codegen_expr(ParsingContext *context,
                                ParsingContext **ctx_next_child,
                                AstNode *curr_expr, CGContext *cg_ctx,
                                FILE *fptr_code) {
    switch (curr_expr->type) {

    case TYPE_VAR_DECLARATION:
        if (cg_ctx->parent_ctx == NULL)
            break;

        if (codegen_verbose)
            fprintf(fptr_code, ";#; Variable Declaration : `%s`\n",
                    curr_expr->child->ast_val.node_symbol);

        AstNode *var_node = NULL;
        int stat = -1;
        var_node = parser_get_var(context, curr_expr->child, &stat);
        if (stat == 0)
            print_error(ERR_COMMON,
                        "Unable to find variable in environment : `%s`",
                        curr_expr->child->ast_val.node_symbol);

        if (strcmp(var_node->ast_val.node_symbol, "ext function") == 0)
            break;

        AstNode *type_node = NULL;
        type_node = parser_get_type(context, var_node, &stat);
        long size_in_bytes = type_node->child->ast_val.val;
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        type_node->ast_val.node_symbol);

        file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_SUB,
                              OPERAND_TYPE_IMM_TO_REG, (int64_t)size_in_bytes,
                              REG_X86_64_RSP);
        cg_ctx->local_offset -= size_in_bytes;
        if (!set_env(&cg_ctx->local_env, curr_expr->child,
                     create_node_int(cg_ctx->local_offset)))
            print_error(ERR_COMMON,
                        "Unable to set locals environment in code gen context "
                        "for : `%s`",
                        curr_expr->child->ast_val.node_symbol);
        break;

    case TYPE_INT:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Literal Integer : %ld\n",
                    curr_expr->ast_val.val);
        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                              OPERAND_TYPE_IMM_TO_REG, curr_expr->ast_val.val,
                              curr_expr->result_reg_desc);
        break;

    case TYPE_VAR_ACCESS:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Variable Access : `%s`\n",
                    curr_expr->ast_val.node_symbol);

        CGContext *var_cg_ctx = cg_ctx;
        stat = -1;
        AstNode *local_var_name;
        while (var_cg_ctx != NULL) {
            local_var_name = get_env_from_sym(
                var_cg_ctx->local_env, curr_expr->ast_val.node_symbol, &stat);
            if (stat)
                break;
            if (var_cg_ctx->parent_ctx != NULL)
                free_node(local_var_name);
            var_cg_ctx = var_cg_ctx->parent_ctx;
        }

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        if (var_cg_ctx == NULL) {
            fprintf(fptr_code, "mov %s(%%rip), %s\n",
                    curr_expr->ast_val.node_symbol,
                    get_reg_name(cg_ctx, curr_expr->result_reg_desc));
        } else {
            // Check if `stat` isn't 0, i.e. the variable was found.
            if (stat == 0)
                print_error(ERR_COMMON,
                            "Unable to find information regarding local "
                            "variable offset for: `%s`",
                            curr_expr->ast_val.node_symbol);
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                                  OPERAND_TYPE_MEM_TO_REG,
                                  local_var_name->ast_val.val, REG_X86_64_RBP,
                                  curr_expr->result_reg_desc);
        }
        break;

    case TYPE_BINARY_OPERATOR:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Binary Operator : %s\n",
                    curr_expr->ast_val.node_symbol);
        // Move the integers on the left and right hand side into different
        // registers.
        // See: https://www.felixcloutier.com/x86/
        target_x86_64_codegen_expr(context, ctx_next_child, curr_expr->child,
                                   cg_ctx, fptr_code);
        target_x86_64_codegen_expr(context, ctx_next_child,
                                   curr_expr->child->next_child, cg_ctx,
                                   fptr_code);

        // Get the names of those registers, which contain the LHS and RHS
        // integers.
        const char *reg_lhs =
            get_reg_name(cg_ctx, curr_expr->child->result_reg_desc);
        const char *reg_rhs =
            get_reg_name(cg_ctx, curr_expr->child->next_child->result_reg_desc);

        if (strcmp(curr_expr->ast_val.node_symbol, ">") == 0) {
            target_x86_64_win_codegen_comp(cg_ctx, curr_expr, COMP_GT,
                                           fptr_code);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "<") == 0) {
            target_x86_64_win_codegen_comp(cg_ctx, curr_expr, COMP_LT,
                                           fptr_code);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "==") == 0) {
            target_x86_64_win_codegen_comp(cg_ctx, curr_expr, COMP_EQ,
                                           fptr_code);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "+") == 0) {
            curr_expr->result_reg_desc =
                curr_expr->child->next_child->result_reg_desc;
            // Add those registers and save the result in the RHS register.
            file_emit_x86_64_inst(
                fptr_code, cg_ctx, INST_X86_64_ADD, OPERAND_TYPE_REG_TO_REG,
                curr_expr->child->result_reg_desc,
                curr_expr->child->next_child->result_reg_desc);

            // De-allocate the LHS register since it is not in use anymore.
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "-") == 0) {
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
            // Subtract those registers and save the result in the LHS register.
            // `sub` operation subtracts the first operand from the second
            // operand, and stores it in the second operand.
            file_emit_x86_64_inst(
                fptr_code, cg_ctx, INST_X86_64_SUB, OPERAND_TYPE_REG_TO_REG,
                curr_expr->child->result_reg_desc,
                curr_expr->child->next_child->result_reg_desc);

            // De-allocate the LHS register since it is not in use anymore.
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "<<") == 0) {
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
            // Since shift left is destructive, we use the expression result
            // register as the LHS register. The RHS or the amount by which the
            // shift left needs to be done is placed into RCX, which is used by
            // the SHL instruction by default and the final value is stored in
            // the LHS register.
            fprintf(fptr_code,
                    "push %%rcx\n"
                    "mov %s, %%rcx\n"
                    "sal %%cl, %s\n"
                    "pop %%rcx\n",
                    reg_rhs, reg_lhs);

            // De-allocate the RHS register since it is not in use anymore.
            reg_dealloc(cg_ctx, curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, ">>") == 0) {
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
            // Since shift right is destructive, we use the expression result
            // register as the LHS register. The RHS or the amount by which the
            // shift right needs to be done is placed into RCX, which is used by
            // the SHL instruction by default and the final value is stored in
            // the LHS register.
            fprintf(fptr_code,
                    "push %%rcx\n"
                    "mov %s, %%rcx\n"
                    "sar %%cl, %s\n"
                    "pop %%rcx\n",
                    reg_rhs, reg_lhs);

            // De-allocate the RHS register since it is not in use anymore.
            reg_dealloc(cg_ctx, curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "*") == 0) {
            curr_expr->result_reg_desc =
                curr_expr->child->next_child->result_reg_desc;
            file_emit_x86_64_inst(
                fptr_code, cg_ctx, INST_X86_64_IMUL, OPERAND_TYPE_REG_TO_REG,
                curr_expr->child->result_reg_desc,
                curr_expr->child->next_child->result_reg_desc);

            // De-allocate the RHS register since it is not in use anymore.
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "/") == 0 ||
                   strcmp(curr_expr->ast_val.node_symbol, "%") == 0) {

            char is_modulus = 0;
            if (strcmp(curr_expr->ast_val.node_symbol, "%") == 0)
                is_modulus = 1;

            // Quotient is saved in RAX, and remainder is saved in RDX.
            // So there registers need to be saved.
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_PUSH,
                                  OPERAND_TYPE_REG, REG_X86_64_RAX);
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_PUSH,
                                  OPERAND_TYPE_REG, REG_X86_64_RDX);

            // Move result into RAX only if it isn't the result register.
            if (curr_expr->child->result_reg_desc != REG_X86_64_RAX)
                file_emit_x86_64_inst(
                    fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                    curr_expr->child->result_reg_desc, REG_X86_64_RAX);

            // Move the value from LHS into RAX. Use signed division instead of
            // unsigned to avoid division error, when using negative numbers.
            // Sign extend the value in RAX to RDX.
            // RDX is used as the 8 high-bytes of a 16 byte number
            // stored in RDX:RAX. Convert 8 bytes into a 16 byte number using
            // `cqto` (convert quad word to octo word).
            fprintf(fptr_code, "cqto\n");

            // Call `idiv` instruction with the RHS register.
            file_emit_x86_64_inst(
                fptr_code, cg_ctx, INST_X86_64_IDIV, OPERAND_TYPE_REG,
                curr_expr->child->next_child->result_reg_desc);

            // Move the result that is stored in RAX, into the expression's
            // result register.
            curr_expr->result_reg_desc = reg_alloc(cg_ctx);
            if (is_modulus) {
                if (curr_expr->result_reg_desc != REG_X86_64_RDX)
                    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                                          OPERAND_TYPE_REG_TO_REG,
                                          REG_X86_64_RDX,
                                          curr_expr->result_reg_desc);
            } else {
                if (curr_expr->result_reg_desc != REG_X86_64_RAX)
                    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                                          OPERAND_TYPE_REG_TO_REG,
                                          REG_X86_64_RAX,
                                          curr_expr->result_reg_desc);
            }

            fprintf(fptr_code, "pop %%rdx\n"
                               "pop %%rax\n");

            // De-allocate the RHS/LHS register since they are not in use
            // anymore. reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
            // reg_dealloc(cg_ctx,
            // curr_expr->child->next_child->result_reg_desc);

        } else
            print_error(ERR_COMMON, "Found unknown binary operator : `%s`",
                        curr_expr->ast_val.node_symbol);
        break;

    case TYPE_FUNCTION_CALL:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Function Call : `%s`\n",
                    curr_expr->child->ast_val.node_symbol);

        // Save RAX because, it may not be preserved after the function call,
        // because the return value is always placed into RAX.
        file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_PUSH,
                              OPERAND_TYPE_REG, REG_X86_64_RAX);

        stat = -1;
        AstNode *func_call_type =
            parser_get_var(context, curr_expr->child, &stat);
        if (!stat)
            print_error(
                ERR_TYPE,
                "Unable to find type information in environment, for: `%s`",
                curr_expr->child->ast_val.node_symbol);

        int param_count = 0;
        AstNode *call_params = curr_expr->child->next_child->child;
        if (strcmp(func_call_type->ast_val.node_symbol, "ext function") == 0) {
            // Save the following registers before using them since they
            // are scratch registers.

            // Put function arguments in RCX, RDX, R8 and R9. If more exist push
            // onto stack in reverse order.
            if (call_params) {
                // Place the first parameter into RCX or XMM0.
                target_x86_64_codegen_expr(context, ctx_next_child, call_params,
                                           cg_ctx, fptr_code);
                file_emit_x86_64_inst(
                    fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                    call_params->result_reg_desc, REG_X86_64_RCX);
                call_params = call_params->next_child;
            }
            if (call_params) {
                // Place the second parameter into RDX or XMM1.
                target_x86_64_codegen_expr(context, ctx_next_child, call_params,
                                           cg_ctx, fptr_code);
                file_emit_x86_64_inst(
                    fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                    call_params->result_reg_desc, REG_X86_64_RDX);
                call_params = call_params->next_child;
            }
            if (call_params) {
                // Place the third parameter into R8 or XMM2.
                target_x86_64_codegen_expr(context, ctx_next_child, call_params,
                                           cg_ctx, fptr_code);
                file_emit_x86_64_inst(
                    fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                    call_params->result_reg_desc, REG_X86_64_R8);
                call_params = call_params->next_child;
            }
            if (call_params) {
                // Place the fourth parameter into R9 or XMM3.
                target_x86_64_codegen_expr(context, ctx_next_child, call_params,
                                           cg_ctx, fptr_code);
                file_emit_x86_64_inst(
                    fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                    call_params->result_reg_desc, REG_X86_64_R9);
                call_params = call_params->next_child;
            }

            // The rest of the paraameters need to be reversed and placed onto
            // the stack.

            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_CALL,
                                  OPERAND_TYPE_SYM,
                                  curr_expr->child->ast_val.node_symbol);
        } else {
            // Push arguments onto the stack in order.
            while (call_params != NULL) {
                target_x86_64_codegen_expr(context, ctx_next_child, call_params,
                                           cg_ctx, fptr_code);
                file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_PUSH,
                                      OPERAND_TYPE_REG,
                                      call_params->result_reg_desc);
                reg_dealloc(cg_ctx, call_params->result_reg_desc);
                call_params = call_params->next_child;
                param_count += 8;
            }
            /**
             * Consider the following assembly code:
             * mov $3, %rax
             * pushq %rax
             * call <func_name>
             * add $8, %rsp
             *
             * When the `call` instruction is executeed, this is the state of
             * the stack |           3           | | call - return address | |
             * Old  RBP        | <-- RSP The `old RBP` comes into the picture
             * because we push RBP onto the stack whenever we define a function.
             * So, now to get to 3, we will travel up 8 bytes to reach the `call
             * - return address`, and 8 more to reach our argument, i.e. 3.
             */

            // Now that we treats functions as variables for calling them we
            // need to var access the name of the "function variable" and call
            // it's result register. See TYPE_FUNCTION for more details.

            target_x86_64_codegen_expr(context, ctx_next_child,
                                       curr_expr->child, cg_ctx, fptr_code);
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_CALL,
                                  OPERAND_TYPE_REG,
                                  curr_expr->child->result_reg_desc);
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
        }

        if (param_count > 0)
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_ADD,
                                  OPERAND_TYPE_IMM_TO_REG, param_count,
                                  REG_X86_64_RSP);

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        if (curr_expr->result_reg_desc != REG_X86_64_RAX) {
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                                  OPERAND_TYPE_REG_TO_REG, REG_X86_64_RAX,
                                  curr_expr->result_reg_desc);

            // Restore the value for RAX from stack.
            fprintf(fptr_code, "pop %%rax\n");
        } else
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_ADD,
                                  OPERAND_TYPE_IMM_TO_REG, (int64_t)8,
                                  REG_X86_64_RSP);
        break;

    case TYPE_FUNCTION:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Function Definition\n");

        ParsingContext *tmp_ctx = context;
        AstNode *func_id = NULL;
        stat = -1;
        while (tmp_ctx != NULL) {
            func_id = get_env_from_val(context->funcs, curr_expr, &stat);
            if (stat)
                break;
            tmp_ctx = tmp_ctx->parent_ctx;
            if (tmp_ctx != NULL)
                free_node(func_id);
        }
        char *func_name = NULL;
        if (stat)
            func_name = func_id->ast_val.node_symbol;
        else
            func_name = gen_label();

        target_x86_64_gnu_as_codegen_func(cg_ctx, context, ctx_next_child,
                                          func_name, curr_expr, fptr_code);

        /**
         * Now that functions are being treated as variables we can use
         * the following assembly to re-assign them to a different function
         * signature and use that to call the actual function.
         *
         *      jmp after_foo_label
         * foo_label:
         *      push %rbp
         *      mov %rsp, %rbp
         *      sub $32, %rsp
         *      mov $69, %rax
         *      add $32, %rsp
         * after_foo_label:
         *      lea foo_label(%rip), %rax
         *      mov %rax, bar(%rip)
         *      mov bar(%rip), %rax
         *      call *%rax
         */

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_LEA,
                              OPERAND_TYPE_SYM_TO_REG, func_name,
                              REG_X86_64_RIP, curr_expr->result_reg_desc);
        break;

    case TYPE_VAR_REASSIGNMENT:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Variable Re-assignment\n");
        AstNode *temp_sym = curr_expr->child;
        while (temp_sym != NULL && temp_sym->type != TYPE_VAR_ACCESS)
            temp_sym = temp_sym->child;
        if (temp_sym == NULL)
            print_error(ERR_COMMON, "Unable to find valid variable access in a "
                                    "variable Re-assignment");

        target_x86_64_codegen_expr(context, ctx_next_child,
                                   curr_expr->child->next_child, cg_ctx,
                                   fptr_code);

        char *res_string = NULL;
        char res_string_free = 0;
        if (curr_expr->child->type == TYPE_VAR_ACCESS) {
            res_string = map_sym_to_addr_win(cg_ctx, temp_sym);
        } else {
            res_string_free = 1;
            target_x86_64_codegen_expr(context, ctx_next_child,
                                       curr_expr->child, cg_ctx, fptr_code);
            const char *res_reg_name =
                get_reg_name(cg_ctx, curr_expr->child->result_reg_desc);
            char *res_string_copy = strdup(res_reg_name);
            size_t total_len = strlen(res_reg_name) + 3;
            res_string = calloc(total_len, sizeof(char));
            snprintf(res_string, total_len, "(%s)", res_string_copy);
            res_string[total_len - 1] = '\0';
            free(res_string_copy);
        }

        fprintf(
            fptr_code, "mov %s, %s\n",
            get_reg_name(cg_ctx, curr_expr->child->next_child->result_reg_desc),
            res_string);
        // De-allocate RHS result register.
        reg_dealloc(cg_ctx, curr_expr->child->next_child->result_reg_desc);

        if (res_string_free) {
            free((char *)res_string);
            // De-allocate LHS result register.
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
        }
        break;

    case TYPE_IF_CONDITION:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; IF Block\n");
        target_x86_64_codegen_expr(context, ctx_next_child, curr_expr->child,
                                   cg_ctx, fptr_code);

        if (codegen_verbose)
            fprintf(fptr_code, ";#; If Condition\n");

        const char *res_reg =
            get_reg_name(cg_ctx, curr_expr->child->result_reg_desc);
        char *else_label = gen_label();
        char *after_else_label = gen_label();
        fprintf(fptr_code, "test %s, %s\n", res_reg, res_reg);
        fprintf(fptr_code, "jz %s\n", else_label);
        reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);

        if (codegen_verbose)
            fprintf(fptr_code, ";#; If Then Body\n");

        ParsingContext *ctx = context;
        ParsingContext *ctx_child = *ctx_next_child;
        if (*ctx_next_child != NULL) {
            ctx = *ctx_next_child;
            ctx_child = (*ctx_next_child)->child;
            *ctx_next_child = (*ctx_next_child)->next_child;
        }

        // The if body comes here.
        AstNode *last_expr = NULL;
        AstNode *if_expr = curr_expr->child->next_child->child;
        while (if_expr != NULL) {
            target_x86_64_codegen_expr(ctx, &ctx_child, if_expr, cg_ctx,
                                       fptr_code);
            if (last_expr != NULL)
                reg_dealloc(cg_ctx, last_expr->result_reg_desc);
            last_expr = if_expr;
            if_expr = if_expr->next_child;
        }

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        file_emit_x86_64_inst(
            fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
            last_expr->result_reg_desc, curr_expr->result_reg_desc);
        reg_dealloc(cg_ctx, last_expr->result_reg_desc);
        fprintf(fptr_code, "jmp %s\n", after_else_label);

        if (codegen_verbose)
            fprintf(fptr_code, ";#; Else Body\n");
        fprintf(fptr_code, "%s:\n", else_label);

        // Else body
        last_expr = NULL;
        AstNode *else_expr = curr_expr->child->next_child->next_child;
        if (else_expr != NULL) {
            ctx = context;
            ctx_child = *ctx_next_child;
            if (*ctx_next_child != NULL) {
                ctx = *ctx_next_child;
                ctx_child = (*ctx_next_child)->child;
                *ctx_next_child = (*ctx_next_child)->next_child;
            }

            else_expr = else_expr->child;
            while (else_expr != NULL) {
                target_x86_64_codegen_expr(ctx, &ctx_child, else_expr, cg_ctx,
                                           fptr_code);
                if (last_expr != NULL)
                    reg_dealloc(cg_ctx, last_expr->result_reg_desc);
                last_expr = else_expr;
                else_expr = else_expr->next_child;
            }

            file_emit_x86_64_inst(
                fptr_code, cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                last_expr->result_reg_desc, curr_expr->result_reg_desc);
            reg_dealloc(cg_ctx, last_expr->result_reg_desc);
        } else {
            // If there is an 'if' statement with no else we need to set the
            // result register for the 'if' statement.
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                                  OPERAND_TYPE_IMM_TO_REG, 0,
                                  curr_expr->result_reg_desc);
        }

        fprintf(fptr_code, "%s:\n", after_else_label);
        break;

    case TYPE_DEREFERENCE:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Dereference\n");
        target_x86_64_codegen_expr(context, ctx_next_child, curr_expr->child,
                                   cg_ctx, fptr_code);
        curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
        break;

    case TYPE_ADDROF:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; AddressOf\n");
        if (curr_expr->child->type == TYPE_ARR_INDEX) {
            target_x86_64_codegen_expr(context, ctx_next_child,
                                       curr_expr->child, cg_ctx, fptr_code);
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
        } else {
            curr_expr->result_reg_desc = reg_alloc(cg_ctx);
            fprintf(fptr_code, "lea %s, %s\n",
                    map_sym_to_addr_win(cg_ctx, curr_expr->child),
                    get_reg_name(cg_ctx, curr_expr->result_reg_desc));
        }
        break;

    case TYPE_ARR_INDEX:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Arr Index : %ld\n", curr_expr->ast_val.val);

        // Get type information for the variable.
        stat = -1;
        AstNode *arr_type = parser_get_var(context, curr_expr->child, &stat);
        if (stat == 0)
            print_error(ERR_COMMON,
                        "Unable to get information for array : `%s`",
                        curr_expr->child->ast_val.node_symbol);

        // Total array size.
        // long arr_size = arr_type->child->ast_val.val;

        // Get the size of the base type.
        stat = -1;
        AstNode *base_type =
            parser_get_type(context, arr_type->child->next_child, &stat);
        if (stat == 0)
            print_error(ERR_COMMON,
                        "Unable to get base type information for array : `%s`",
                        curr_expr->child->ast_val.node_symbol);

        // Base type size.
        long base_type_size = base_type->child->ast_val.val;
        free_node(base_type);

        long arr_offset = curr_expr->ast_val.val * base_type_size;

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        fprintf(fptr_code, "lea %s, %s\n",
                map_sym_to_addr_win(cg_ctx, curr_expr->child),
                get_reg_name(cg_ctx, curr_expr->result_reg_desc));

        if (arr_offset)
            file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_ADD,
                                  OPERAND_TYPE_IMM_TO_REG, arr_offset,
                                  curr_expr->result_reg_desc);

        // fprintf(fptr_code, "mov (%s), %s\n",
        //         get_reg_name(cg_ctx, curr_expr->result_reg_desc),
        //         get_reg_name(cg_ctx, curr_expr->result_reg_desc));
        break;

    default:
        break;
    }
}

CGContext *create_from_codegen_context(CGContext *parent) {
    if (parent == NULL)
        print_error(ERR_COUNT,
                    "Encountered NULL parent in create_from_codegen_context()");

    CGContext *new_ctx = NULL;
    if (parent->target_fmt == TARGET_FMT_X86_64_GNU_AS) {
        if (parent->target_call_conv == TARGET_CALL_CONV_WIN)
            new_ctx = create_codegen_context_gnu_as_win(parent);
        else if (parent->target_call_conv == TARGET_CALL_CONV_LINUX) {
            // cg_ctx = create_codegen_context_gnu_as_linux(cg_ctx);
        }
    }

    if (new_ctx == NULL)
        print_error(ERR_COMMON, "Could not create from codegen context");

    return new_ctx;
}

void target_x86_64_gnu_as_codegen_func(CGContext *cg_ctx,
                                       ParsingContext *context,
                                       ParsingContext **ctx_next_child,
                                       char *func_name, AstNode *func,
                                       FILE *fptr_code) {

    cg_ctx = create_from_codegen_context(cg_ctx);

    /**
     * Storing the offset for parameters passed to the function
     * so that we know, which register to use for passing them
     * to the function.
     *
     * We also set the locals environment to have the name of the
     * parameter bound to an integer with the stack offset.
     * Example:
     * jmp afterfoo
     * foo:
     *     push %rbp
     *     mov %rsp, %rbp
     *
     *     pop %rbp
     *     ret
     * Due to the first and second instructions in the function, we
     * have moved 8 bytes twice in the stack, i.e. 16 bytes,
     * so we will need to account for them while binding the value to
     * the parameter names. Hence notice 'param_cnt' starts from '2',
     * to make space for the first and second instructions.
     */
    AstNode *func_param_list = func->child->next_child->child;
    long param_cnt = 1;
    while (func_param_list != NULL) {
        param_cnt++;
        if (!set_env(&(cg_ctx->local_env), func_param_list->child,
                     create_node_int(param_cnt * 8)))
            print_error(ERR_COMMON,
                        "Unable to set locals environment in code gen context "
                        "for : `%s`",
                        func_param_list->child->ast_val.node_symbol);
        func_param_list = func_param_list->next_child;
    }

    // Function protection
    fprintf(fptr_code, "jmp after%s\n", func_name);

    fprintf(fptr_code, "%s:\n", func_name);

    // Function header.
    fprintf(fptr_code, "%s", FUNC_HEADER_x86_64);

    ParsingContext *ctx = context;
    ParsingContext *ctx_child = *ctx_next_child;
    if (*ctx_next_child != NULL) {
        ctx = *ctx_next_child;
        ctx_child = (*ctx_next_child)->child;
        *ctx_next_child = (*ctx_next_child)->next_child;
    }
    AstNode *temp_expr = func->child->next_child->next_child->child;
    AstNode *last_expr = NULL;
    while (temp_expr != NULL) {
        target_x86_64_codegen_expr(ctx, &ctx_child, temp_expr, cg_ctx,
                                   fptr_code);
        reg_dealloc(cg_ctx, temp_expr->result_reg_desc);
        last_expr = temp_expr;
        temp_expr = temp_expr->next_child;
    }

    // Function footer.
    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                          OPERAND_TYPE_REG_TO_REG, last_expr->result_reg_desc,
                          REG_X86_64_RAX);
    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_ADD,
                          OPERAND_TYPE_IMM_TO_REG, -cg_ctx->local_offset,
                          REG_X86_64_RSP);
    fprintf(fptr_code, "%s", FUNC_FOOTER_x86_64);

    // Jump after function protection
    fprintf(fptr_code, "after%s:\n", func_name);
    cg_ctx = cg_ctx->parent_ctx;
}

void target_x86_64_codegen_prog(ParsingContext *context, AstNode *program,
                                CGContext *cg_ctx, FILE *fptr_code) {

    fprintf(fptr_code, ".section .data\n");

    IdentifierBind *temp_var_bind = context->vars->binding;
    AstNode *temp_var_type_id = NULL;
    int status = -1;
    while (temp_var_bind != NULL) {
        temp_var_type_id = node_alloc();
        *temp_var_type_id = *temp_var_bind->id_val;
        // temp_var_type_id->child = NULL;
        // temp_var_type_id->next_child = NULL;
        AstNode *var = parser_get_type(context, temp_var_type_id, &status);
        if (!status)
            print_error(ERR_COMMON,
                        "Unable to retrieve value from environment for : `%s`",
                        temp_var_type_id->ast_val.node_symbol);

        if (strcmp(temp_var_type_id->ast_val.node_symbol, "ext function") != 0)
            fprintf(fptr_code, "%s: .space %ld\n",
                    temp_var_bind->identifier->ast_val.node_symbol,
                    var->child->ast_val.val);

        temp_var_bind = temp_var_bind->next_id_bind;
        free_node(temp_var_type_id);
    }

    fprintf(fptr_code,
            ".section .text\n"
            ".global main\n"
            "main:\n"
            "%s",
            FUNC_HEADER_x86_64);

    ParsingContext *ctx_next_child = context->child;
    AstNode *curr_expr = program->child;
    AstNode *last_expr = NULL;
    while (curr_expr != NULL) {
        if (curr_expr->type == TYPE_NULL) {
            curr_expr = curr_expr->next_child;
            continue;
        }
        target_x86_64_codegen_expr(context, &ctx_next_child, curr_expr, cg_ctx,
                                   fptr_code);
        reg_dealloc(cg_ctx, curr_expr->result_reg_desc);
        last_expr = curr_expr;
        curr_expr = curr_expr->next_child;
    }

    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_MOV,
                          OPERAND_TYPE_REG_TO_REG, last_expr->result_reg_desc,
                          REG_X86_64_RAX);
    file_emit_x86_64_inst(fptr_code, cg_ctx, INST_X86_64_ADD,
                          OPERAND_TYPE_IMM_TO_REG, -cg_ctx->local_offset,
                          REG_X86_64_RSP);
    fprintf(fptr_code, "%s", FUNC_FOOTER_x86_64);
}

void target_codegen(ParsingContext *context, AstNode *program,
                    char *output_file_path, TargetFormat type,
                    TargetCallingConvention call_conv) {
    if (context == NULL || program == NULL)
        print_error(ERR_COMMON,
                    "NULL program node passed for code generation to "
                    "`target_codegen()`");

    FILE *fptr_code = NULL;
    if (output_file_path == NULL) {
        fptr_code = fopen("code_gen.s", "w");
        if (fptr_code == NULL)
            print_error(ERR_FILE_OPEN, "Unable to open default file for code "
                                       "generation : `code_gen.s`");
    } else {
        fptr_code = fopen(output_file_path, "w");
        if (fptr_code == NULL)
            print_error(ERR_FILE_OPEN,
                        "Unable to open file for code generation : `%s`",
                        output_file_path);
    }

    CGContext *cg_ctx = NULL;

    if (call_conv == TARGET_CALL_CONV_WIN)
        cg_ctx = create_codegen_context_gnu_as_win(NULL);
    else if (call_conv == TARGET_CALL_CONV_LINUX) {
        print_error(ERR_ARGS, "LINUX calling convention not yet supported :(");
    } else {
        print_error(ERR_ARGS, "Encountered invalid calling convention");
    }

    if (type == TARGET_FMT_DEFAULT || type == TARGET_FMT_X86_64_GNU_AS)
        target_x86_64_codegen_prog(context, program, cg_ctx, fptr_code);

    free_codegen_context(cg_ctx);
    fclose(fptr_code);
}
