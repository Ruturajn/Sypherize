#include "../inc/code_gen.h"
#include "../inc/arch/platforms.h"
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

void target_codegen_func(CGContext *cg_ctx, ParsingContext *context,
                         ParsingContext **ctx_next_child, char *func_name,
                         AstNode *func);
char codegen_verbose = 1;

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

SymToAddr map_sym_to_addr(CGContext *cg_ctx, AstNode *sym_node) {
    if (sym_node == NULL || sym_node->ast_val.node_symbol == NULL ||
        cg_ctx == NULL)
        print_error(ERR_DEV, "map_sym_to_addr() : NULL symbol/codegen context");

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

void target_codegen_expr(ParsingContext *context,
                         ParsingContext **ctx_next_child, AstNode *curr_expr,
                         CGContext *cg_ctx, FILE *fptr_code) {
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

        code_gen_allocate_on_stack(cg_ctx, size_in_bytes);
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
        curr_expr->result_reg_desc =
            code_gen_get_imm(cg_ctx, curr_expr->ast_val.val);
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

        if (var_cg_ctx == NULL) {
            curr_expr->result_reg_desc =
                code_gen_get_global(cg_ctx, curr_expr->ast_val.node_symbol);

        } else {
            // Check if `stat` isn't 0, i.e. the variable was found.
            if (stat == 0)
                print_error(ERR_COMMON,
                            "Unable to find information regarding local "
                            "variable offset for: `%s`",
                            curr_expr->ast_val.node_symbol);
            curr_expr->result_reg_desc =
                code_gen_get_local(cg_ctx, local_var_name->ast_val.val);
        }
        break;

    case TYPE_BINARY_OPERATOR:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Binary Operator : %s\n",
                    curr_expr->ast_val.node_symbol);
        // Move the integers on the left and right hand side into different
        // registers.
        // See: https://www.felixcloutier.com/x86/
        target_codegen_expr(context, ctx_next_child, curr_expr->child, cg_ctx,
                            fptr_code);
        target_codegen_expr(context, ctx_next_child,
                            curr_expr->child->next_child, cg_ctx, fptr_code);

        if (strcmp(curr_expr->ast_val.node_symbol, ">") == 0) {
            code_gen_compare(cg_ctx, COMP_GT, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "<") == 0) {
            code_gen_compare(cg_ctx, COMP_LT, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "==") == 0) {
            code_gen_compare(cg_ctx, COMP_EQ, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "+") == 0) {
            curr_expr->result_reg_desc =
                code_gen_add(cg_ctx, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "-") == 0) {
            // Subtract those registers and save the result in the LHS register.
            // `sub` operation subtracts the first operand from the second
            // operand, and stores it in the second operand.
            curr_expr->result_reg_desc =
                code_gen_sub(cg_ctx, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "<<") == 0) {
            // Since shift left is destructive, we use the expression result
            // register as the LHS register. The RHS or the amount by which the
            // shift left needs to be done is placed into RCX, which is used by
            // the SHL instruction by default and the final value is stored in
            // the LHS register.
            curr_expr->result_reg_desc = code_gen_shift_left(
                cg_ctx, curr_expr->child->result_reg_desc,
                curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, ">>") == 0) {
            // Since shift right is destructive, we use the expression result
            // register as the LHS register. The RHS or the amount by which the
            // shift right needs to be done is placed into RCX, which is used by
            // the SHL instruction by default and the final value is stored in
            // the LHS register.
            curr_expr->result_reg_desc = code_gen_shift_right_arithmetic(
                cg_ctx, curr_expr->child->result_reg_desc,
                curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "*") == 0) {
            curr_expr->result_reg_desc =
                code_gen_mul(cg_ctx, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "/") == 0) {
            curr_expr->result_reg_desc =
                code_gen_div(cg_ctx, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else if (strcmp(curr_expr->ast_val.node_symbol, "%") == 0) {
            curr_expr->result_reg_desc =
                code_gen_mod(cg_ctx, curr_expr->child->result_reg_desc,
                             curr_expr->child->next_child->result_reg_desc);

        } else
            print_error(ERR_COMMON, "Found unknown binary operator : `%s`",
                        curr_expr->ast_val.node_symbol);
        break;

    case TYPE_FUNCTION_CALL:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Function Call : `%s`\n",
                    curr_expr->child->ast_val.node_symbol);

        code_gen_setup_func_call(cg_ctx);

        stat = -1;
        AstNode *func_call_type =
            parser_get_var(context, curr_expr->child, &stat);
        if (!stat)
            print_error(
                ERR_TYPE,
                "Unable to find type information in environment, for: `%s`",
                curr_expr->child->ast_val.node_symbol);

        AstNode *call_params = curr_expr->child->next_child->child;
        if (strcmp(func_call_type->ast_val.node_symbol, "ext function") == 0) {
            // Save the following registers before using them since they
            // are scratch registers.

            // Put function arguments in RCX, RDX, R8 and R9. If more exist push
            // onto stack in reverse order.
            while (call_params != NULL) {
                target_codegen_expr(context, ctx_next_child, call_params,
                                    cg_ctx, fptr_code);
                code_gen_ext_func_arg(cg_ctx, call_params->result_reg_desc);
                call_params = call_params->next_child;
            }

            // The rest of the paraameters need to be reversed and placed onto
            // the stack.

            code_gen_ext_func_call(cg_ctx,
                                   curr_expr->child->ast_val.node_symbol);

        } else {
            // Push arguments onto the stack in order.
            while (call_params != NULL) {
                target_codegen_expr(context, ctx_next_child, call_params,
                                    cg_ctx, fptr_code);
                code_gen_func_arg(cg_ctx, call_params->result_reg_desc);
                reg_dealloc(cg_ctx, call_params->result_reg_desc);
                call_params = call_params->next_child;
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

            target_codegen_expr(context, ctx_next_child, curr_expr->child,
                                cg_ctx, fptr_code);
            curr_expr->result_reg_desc =
                code_gen_func_call(cg_ctx, curr_expr->child->result_reg_desc);
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
        }

        code_gen_cleanup(cg_ctx);

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

        target_codegen_func(cg_ctx, context, ctx_next_child, func_name,
                            curr_expr);

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
        code_gen_get_global_addr_into(cg_ctx, func_name,
                                      curr_expr->result_reg_desc);
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

        target_codegen_expr(context, ctx_next_child,
                            curr_expr->child->next_child, cg_ctx, fptr_code);

        if (curr_expr->child->type == TYPE_VAR_ACCESS) {
            SymToAddr addr = map_sym_to_addr(cg_ctx, curr_expr->child);
            switch (addr.type) {
            case SYM_ADDR_GLOBAL:
                code_gen_store_global(
                    cg_ctx, addr.val.global,
                    curr_expr->child->next_child->result_reg_desc);
                break;
            case SYM_ADDR_LOCAL:
                code_gen_store_local(
                    cg_ctx, addr.val.local,
                    curr_expr->child->next_child->result_reg_desc);
                break;
            }

        } else {
            target_codegen_expr(context, ctx_next_child, curr_expr->child,
                                cg_ctx, fptr_code);
            code_gen_store(cg_ctx,
                           curr_expr->child->next_child->result_reg_desc,
                           curr_expr->child->result_reg_desc);

            // De-allocate RHS and LHS result register.
            reg_dealloc(cg_ctx, curr_expr->child->next_child->result_reg_desc);
            reg_dealloc(cg_ctx, curr_expr->child->result_reg_desc);
        }
        break;

    case TYPE_IF_CONDITION:;
        if (codegen_verbose)
            fprintf(fptr_code, ";#; IF Block\n");
        target_codegen_expr(context, ctx_next_child, curr_expr->child, cg_ctx,
                            fptr_code);

        if (codegen_verbose)
            fprintf(fptr_code, ";#; If Condition\n");

        char *else_label = gen_label();
        char *after_else_label = gen_label();
        code_gen_branch_if_zero(cg_ctx, curr_expr->child->result_reg_desc,
                                else_label);
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
            target_codegen_expr(ctx, &ctx_child, if_expr, cg_ctx, fptr_code);
            if (last_expr != NULL)
                reg_dealloc(cg_ctx, last_expr->result_reg_desc);
            last_expr = if_expr;
            if_expr = if_expr->next_child;
        }

        curr_expr->result_reg_desc = reg_alloc(cg_ctx);
        code_gen_copy_reg(cg_ctx, last_expr->result_reg_desc,
                          curr_expr->result_reg_desc);
        reg_dealloc(cg_ctx, last_expr->result_reg_desc);
        code_gen_branch(cg_ctx, after_else_label);

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
                target_codegen_expr(ctx, &ctx_child, else_expr, cg_ctx,
                                    fptr_code);
                if (last_expr != NULL)
                    reg_dealloc(cg_ctx, last_expr->result_reg_desc);
                last_expr = else_expr;
                else_expr = else_expr->next_child;
            }

            code_gen_copy_reg(cg_ctx, last_expr->result_reg_desc,
                              curr_expr->result_reg_desc);
            reg_dealloc(cg_ctx, last_expr->result_reg_desc);
        } else {
            // If there is an 'if' statement with no else we need to set the
            // result register for the 'if' statement.
            code_gen_zero_out_reg(cg_ctx, curr_expr->result_reg_desc);
        }

        fprintf(fptr_code, "%s:\n", after_else_label);
        break;

    case TYPE_DEREFERENCE:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; Dereference\n");
        target_codegen_expr(context, ctx_next_child, curr_expr->child, cg_ctx,
                            fptr_code);
        curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
        break;

    case TYPE_ADDROF:
        if (codegen_verbose)
            fprintf(fptr_code, ";#; AddressOf\n");
        if (curr_expr->child->type == TYPE_ARR_INDEX) {
            target_codegen_expr(context, ctx_next_child, curr_expr->child,
                                cg_ctx, fptr_code);
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
        } else {
            SymToAddr addr = map_sym_to_addr(cg_ctx, curr_expr->child);
            switch (addr.type) {
            case SYM_ADDR_GLOBAL:
                curr_expr->result_reg_desc =
                    code_gen_get_global_addr(cg_ctx, addr.val.global);
                break;
            case SYM_ADDR_LOCAL:
                curr_expr->result_reg_desc =
                    code_gen_get_local_addr(cg_ctx, addr.val.local);
                break;
            }
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

        SymToAddr addr = map_sym_to_addr(cg_ctx, curr_expr->child);
        switch (addr.type) {
        case SYM_ADDR_GLOBAL:
            curr_expr->result_reg_desc =
                code_gen_get_global_addr(cg_ctx, addr.val.global);
            break;
        case SYM_ADDR_LOCAL:
            curr_expr->result_reg_desc =
                code_gen_get_local_addr(cg_ctx, addr.val.local);
            break;
        }

        if (arr_offset)
            code_gen_add_imm(cg_ctx, arr_offset, curr_expr->result_reg_desc);

        // fprintf(fptr_code, "mov (%s), %s\n",
        //         get_reg_name(cg_ctx, curr_expr->result_reg_desc),
        //         get_reg_name(cg_ctx, curr_expr->result_reg_desc));
        break;

    default:
        break;
    }
}

#define LABEL_BUFFER_SIZE 1024

void target_codegen_func(CGContext *cg_ctx, ParsingContext *context,
                         ParsingContext **ctx_next_child, char *func_name,
                         AstNode *func) {

    cg_ctx = create_cgcontext_child(cg_ctx);

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
    char after_label_buffer[LABEL_BUFFER_SIZE] = {0};
    snprintf(after_label_buffer, sizeof(after_label_buffer), "after%s",
             func_name);
    code_gen_branch(cg_ctx, after_label_buffer);

    fprintf(cg_ctx->fptr_code, "%s:\n", func_name);

    // Function header.
    code_gen_func_header(cg_ctx);

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
        target_codegen_expr(ctx, &ctx_child, temp_expr, cg_ctx,
                            cg_ctx->fptr_code);
        reg_dealloc(cg_ctx, temp_expr->result_reg_desc);
        last_expr = temp_expr;
        temp_expr = temp_expr->next_child;
    }

    // Function footer.
    code_gen_set_func_ret_val(cg_ctx, last_expr->result_reg_desc);

    code_gen_func_footer(cg_ctx);

    // Jump after function protection
    fprintf(cg_ctx->fptr_code, "after%s:\n", func_name);

    free_cgcontext(cg_ctx);
}

void target_codegen_prog(ParsingContext *context, AstNode *program,
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

    code_gen_set_entry_point(cg_ctx);

    ParsingContext *ctx_next_child = context->child;
    AstNode *curr_expr = program->child;
    AstNode *last_expr = NULL;
    while (curr_expr != NULL) {
        if (curr_expr->type == TYPE_NULL) {
            curr_expr = curr_expr->next_child;
            continue;
        }
        target_codegen_expr(context, &ctx_next_child, curr_expr, cg_ctx,
                            fptr_code);
        reg_dealloc(cg_ctx, curr_expr->result_reg_desc);
        last_expr = curr_expr;
        curr_expr = curr_expr->next_child;
    }

    if (last_expr != NULL)
        code_gen_set_func_ret_val(cg_ctx, last_expr->result_reg_desc);

    code_gen_func_footer(cg_ctx);
}

void target_codegen(ParsingContext *context, AstNode *program,
                    char *output_file_path, TargetFormat type,
                    TargetAssemblyDialect dialect,
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

    CGContext *cg_ctx = create_cgcontext(type, call_conv, dialect, fptr_code);

    target_codegen_prog(context, program, cg_ctx, fptr_code);

    free_cgcontext(cg_ctx);
    fclose(fptr_code);
}
