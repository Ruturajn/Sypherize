#include "../inc/code_gen.h"
#include "../inc/ast_funcs.h"

char label_arr[LABEL_ARR_SIZE];
int label_cnt = 0;
int label_idx = 0;

char sym_arr[SYM_ARR_SIZE];
int sym_cnt = 0;
int sym_idx = 0;

Reg *reg_create_new(char *reg_name) {
    Reg *new_reg = calloc(1, sizeof(Reg));
    CHECK_NULL(new_reg, "Unable to allocate memory for a new register", NULL);
    new_reg->reg_name = reg_name;
    new_reg->next_reg = NULL;
    return new_reg;
}

void reg_add_new(Reg **reg_head, char *reg_name) {
    if (*reg_head == NULL) {
        *reg_head = reg_create_new(reg_name);
        return;
    }

    Reg *temp = *reg_head;
    while (temp->next_reg != NULL) {
        temp = temp->next_reg;
    }
    temp->next_reg = reg_create_new(reg_name);
}

void reg_free(Reg *reg_head) {
    Reg *temp = NULL;
    while (temp != NULL) {
        temp = reg_head;
        reg_head = reg_head->next_reg;
        if (temp->reg_name != NULL)
            free(temp->reg_name);
        free(temp);
    }
}

RegDescriptor reg_alloc(Reg *reg_head) {
    Reg *temp = reg_head;
    RegDescriptor curr_reg = 0;
    while (temp != NULL) {
        if (!temp->reg_in_use) {
            temp->reg_in_use = 1;
            return curr_reg;
        }
        temp = temp->next_reg;
        curr_reg++;
    }
    print_error(ERR_MEM, "Unable to allocate memory for a new register", NULL,
                0);
    return curr_reg;
}

char *get_reg_name(RegDescriptor reg_desc, Reg *reg_head) {
    Reg *temp = reg_head;
    RegDescriptor temp_desc = reg_desc;
    while (temp != NULL) {
        if (temp_desc == 0)
            return temp->reg_name;
        temp = temp->next_reg;
        temp_desc--;
    }
    print_error(ERR_COMMON,
                "Unable to find register name for register descriptor : `%d`",
                NULL, reg_desc);
    return NULL;
}

void reg_dealloc(Reg *reg_head, RegDescriptor reg_desc) {
    Reg *temp = reg_head;
    RegDescriptor temp_desc = reg_desc;
    while (temp != NULL) {
        if (temp_desc == 0) {
            temp->reg_in_use = 0;
            return;
        }
        temp = temp->next_reg;
        temp_desc--;
    }
    print_error(
        ERR_COMMON,
        "Unable to de-allocate register with register descriptor : `%d`", NULL,
        reg_desc);
}

char *gen_label() {
    char *label = label_arr + label_idx;
    label_idx = snprintf(label, LABEL_ARR_SIZE - label_idx, ".L%d", label_cnt);
    label_idx++;
    if (label_idx > LABEL_ARR_SIZE) {
        label_idx = 0;
        label = gen_label();
    }
    label_cnt++;
    return label;
}

char *map_sym_to_addr_win(AstNode *sym_node) {
    char *sym = sym_arr + sym_idx;
    sym_idx = snprintf(sym, SYM_ARR_SIZE - sym_idx, "%s(%%rip)",
                       sym_node->ast_val.node_symbol);
    sym_idx++;
    if (sym_idx > LABEL_ARR_SIZE) {
        sym_idx = 0;
        sym = map_sym_to_addr_win(sym_node);
    }
    sym_cnt++;
    return sym;
}

CGContext *create_codegen_context(CGContext *parent_ctx) {
    CGContext *new_ctx = calloc(1, sizeof(CGContext));
    CHECK_NULL(new_ctx, "Unable to allocate memory for new codegen context",
               NULL);
    new_ctx->parent_ctx = parent_ctx;
    new_ctx->local_env = create_env(NULL);
    return new_ctx;
}

void print_regs(Reg *reg_head) {
    Reg *temp = reg_head;
    while (temp != NULL) {
        printf("REG: %s, USE: %d\n", temp->reg_name, temp->reg_in_use);
        temp = temp->next_reg;
    }
}

void target_x86_64_win_codegen_expr(Reg *reg_head, ParsingContext *context,
                                    AstNode *curr_expr, CGContext *cg_ctx,
                                    FILE *fptr_code) {
    switch (curr_expr->type) {
    case TYPE_VAR_DECLARATION:
        break;
    case TYPE_INT:
        curr_expr->result_reg_desc = reg_alloc(reg_head);
        fprintf(fptr_code, "movq $%ld, %s\n", curr_expr->ast_val.val,
                get_reg_name(curr_expr->result_reg_desc, reg_head));
        break;
    case TYPE_BINARY_OPERATOR:;
        // Move the integers on the left and right hand side into different
        // registers.
        target_x86_64_win_codegen_expr(reg_head, context, curr_expr->child,
                                       cg_ctx, fptr_code);
        target_x86_64_win_codegen_expr(
            reg_head, context, curr_expr->child->next_child, cg_ctx, fptr_code);

        // Get the names of those registers, which contain the LHS and RHS
        // integers.
        char *reg_lhs =
            get_reg_name(curr_expr->child->result_reg_desc, reg_head);
        char *reg_rhs = get_reg_name(
            curr_expr->child->next_child->result_reg_desc, reg_head);
        if (strcmp(curr_expr->ast_val.node_symbol, "+") == 0) {
            curr_expr->result_reg_desc =
                curr_expr->child->next_child->result_reg_desc;
            // Add those registers and save the result in the RHS register.
            fprintf(fptr_code, "add %s, %s\n", reg_lhs, reg_rhs);

            // De-allocate the LHS register since it is not in use anymore.
            reg_dealloc(reg_head, curr_expr->child->result_reg_desc);
        } else if (strcmp(curr_expr->ast_val.node_symbol, "-") == 0) {
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
            // Subtract those registers and save the result in the LHS register.
            // `sub` operation subtracts the second operand from the first
            // operand, and stores it in the first operand.
            fprintf(fptr_code, "sub %s, %s\n", reg_lhs, reg_rhs);

            // De-allocate the RHS register since it is not in use anymore.
            reg_dealloc(reg_head,
                        curr_expr->child->next_child->result_reg_desc);
        } else if (strcmp(curr_expr->ast_val.node_symbol, "*") == 0) {
            curr_expr->result_reg_desc = curr_expr->child->result_reg_desc;
            // Subtract those registers and save the result in the LHS register.
            // `sub` operation subtracts the second operand from the first
            // operand, and stores it in the first operand.
            fprintf(fptr_code, "imul %s, %s\n", reg_lhs, reg_rhs);

            // De-allocate the RHS register since it is not in use anymore.
            reg_dealloc(reg_head,
                        curr_expr->child->next_child->result_reg_desc);
        }
        break;
    case TYPE_FUNCTION:;
        if (cg_ctx->parent_ctx == NULL) {
            break;
        }
        char *lambda_func_name = gen_label();
        target_x86_64_win_codegen_func(reg_head, cg_ctx, context,
                                       lambda_func_name, curr_expr, fptr_code);
        break;
    case TYPE_VAR_REASSIGNMENT:
        if (cg_ctx->parent_ctx != NULL) {
            // print_error("Local variable code gen not implemented", 1, NULL);
            break;
        } else {
            if (curr_expr->child->next_child->type == TYPE_INT) {
                fprintf(fptr_code, "movq $%ld, %s\n",
                        curr_expr->child->next_child->ast_val.val,
                        map_sym_to_addr_win(curr_expr->child));
            } else {
                target_x86_64_win_codegen_expr(reg_head, context,
                                               curr_expr->child->next_child,
                                               cg_ctx, fptr_code);
                char *res_reg = get_reg_name(
                    curr_expr->child->next_child->result_reg_desc, reg_head);
                fprintf(fptr_code, "mov %s, %s\n", res_reg,
                        map_sym_to_addr_win(curr_expr->child));
                reg_dealloc(reg_head,
                            curr_expr->child->next_child->result_reg_desc);
            }
        }
        break;
    default:
        break;
    }
}

void target_x86_64_win_codegen_func(Reg *reg_head, CGContext *cg_ctx,
                                    ParsingContext *context, char *func_name,
                                    AstNode *func, FILE *fptr_code) {
    cg_ctx = create_codegen_context(cg_ctx);

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
    AstNode *func_param_list = func->child->child;
    long param_cnt = 1;
    while (func_param_list != NULL) {
        param_cnt++;
        if (!set_env(&(cg_ctx->local_env), func_param_list->child,
                     create_node_int(-param_cnt * 8)))
            print_error(ERR_COMMON,
                        "Unable to set locals environment in code gen context "
                        "for : `%s`",
                        func_param_list->child->ast_val.node_symbol, 0);
        func_param_list = func_param_list->next_child;
    }

    // Function protection
    fprintf(fptr_code, "jmp after%s\n", func_name);

    fprintf(fptr_code, "%s:\n", func_name);

    // Function header.
    fprintf(fptr_code, "%s", FUNC_HEADER_x86_64);

    AstNode *temp_expr = func->child->next_child->next_child->child;
    while (temp_expr != NULL) {
        target_x86_64_win_codegen_expr(reg_head, context, temp_expr, cg_ctx,
                                       fptr_code);
        temp_expr = temp_expr->next_child;
    }

    // Function footer.
    fprintf(fptr_code, "%s", FUNC_FOOTER_x86_64);

    // Jump after function protection
    fprintf(fptr_code, "after%s:\n", func_name);
    cg_ctx = cg_ctx->parent_ctx;
}

void target_x86_64_win_codegen_prog(ParsingContext *context, AstNode *program,
                                    CGContext *cg_ctx, FILE *fptr_code) {

    Reg *reg_head = reg_create_new("%rax");
    reg_add_new(&reg_head, "%r10");
    reg_add_new(&reg_head, "%r11");

    fprintf(fptr_code, ".section .data\n");

    IdentifierBind *temp_var_bind = context->vars->binding;
    int status = -1;
    while (temp_var_bind != NULL) {
        AstNode *var =
            get_env(context->env_type, temp_var_bind->id_val, &status);
        if (!status)
            print_error(ERR_COMMON,
                        "Unable to retrieve value from environment for : `%s`",
                        temp_var_bind->id_val->ast_val.node_symbol, 0);

        fprintf(fptr_code, "%s: .space %ld\n",
                temp_var_bind->identifier->ast_val.node_symbol,
                var->child->ast_val.val);

        temp_var_bind = temp_var_bind->next_id_bind;
    }

    fprintf(fptr_code, ".section .text\n");

    IdentifierBind *temp_bind = context->funcs->binding;
    status = -1;
    while (temp_bind != NULL) {
        target_x86_64_win_codegen_func(
            reg_head, cg_ctx, context,
            temp_bind->identifier->ast_val.node_symbol, temp_bind->id_val,
            fptr_code);

        temp_bind = temp_bind->next_id_bind;
    }

    fprintf(fptr_code,
            ".global main\n"
            "main:\n"
            "%s",
            FUNC_HEADER_x86_64);

    AstNode *curr_expr = program->child;
    AstNode *last_expr = NULL;
    while (curr_expr != NULL) {
        target_x86_64_win_codegen_expr(reg_head, context, curr_expr, cg_ctx,
                                       fptr_code);
        last_expr = curr_expr;
        curr_expr = curr_expr->next_child;
    }

    if (strcmp(get_reg_name(last_expr->result_reg_desc, reg_head), "%rax"))
        fprintf(fptr_code, "mov %s, %%rax\n",
                get_reg_name(last_expr->result_reg_desc, reg_head));
    fprintf(fptr_code, "%s", FUNC_FOOTER_x86_64);

    reg_free(reg_head);
}

void target_codegen(ParsingContext *context, AstNode *program,
                    TargetType type) {
    if (context == NULL || program == NULL)
        print_error(ERR_COMMON,
                    "NULL program node passed for code generation to "
                    "`target_codegen()`",
                    NULL, 0);

    FILE *fptr_code = fopen("code_gen.s", "w");

    CGContext *cg_ctx = create_codegen_context(NULL);

    if (type == TARGET_DEFAULT || type == TARGET_x86_64_WIN)
        target_x86_64_win_codegen_prog(context, program, cg_ctx, fptr_code);

    fclose(fptr_code);
}
