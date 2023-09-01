#include "../../inc/arch/platforms.h"
#include "../../inc/arch/x86_64/code_gen_x86_64.h"

CGContext *create_cgcontext(TargetFormat fmt, TargetCallingConvention call_conv,
                            TargetAssemblyDialect dialect, FILE *fptr) {

    CGContext *cg_ctx = NULL;

    if (fmt == TARGET_FMT_X86_64_GNU_AS) {
        if (call_conv == TARGET_CALL_CONV_WIN)
            cg_ctx = create_codegen_context_gnu_as_win(NULL);
        else if (call_conv == TARGET_CALL_CONV_LINUX) {
            print_error(ERR_ARGS,
                        "LINUX calling convention not yet supported :(");
        } else {
            print_error(ERR_ARGS, "Encountered invalid calling convention");
        }
    } else
        print_error(ERR_COMMON,
                    "Unrecognized target output format in `create_cgcontext`");

    cg_ctx->fptr_code = fptr;
    cg_ctx->target_asm_dialect = dialect;

    return cg_ctx;
}

CGContext *create_cgcontext_child(CGContext *parent_ctx) {

    if (parent_ctx == NULL)
        print_error(ERR_COMMON,
                    "NULL parent_ctx passed to create_cgcontext_child()");

    CGContext *new_ctx = NULL;
    switch (parent_ctx->target_fmt) {
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in create_cgcontext_child()");
        break;
    case TARGET_FMT_X86_64_GNU_AS:
        switch (parent_ctx->target_call_conv) {
        case TARGET_CALL_CONV_WIN:
            new_ctx = create_codegen_context_gnu_as_win(parent_ctx);
            break;
        case TARGET_CALL_CONV_LINUX:
            print_error(ERR_COMMON, "LINUX calling convention not yet "
                                    "supported in create_cgcontext_child()");
            break;
        default:
            print_error(ERR_COMMON, "Encountered unknown target_call_conv in "
                                    "create_cgcontext_child()");
            break;
        }
        break;
    }
    return new_ctx;
}

void free_cgcontext_child(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in create_cgcontext_child()");
        break;
    case TARGET_FMT_X86_64_GNU_AS:
        switch (cg_ctx->target_call_conv) {
        case TARGET_CALL_CONV_WIN:
            free_codegen_context_gnu_as_win(cg_ctx);
            break;
        case TARGET_CALL_CONV_LINUX:
            print_error(ERR_COMMON, "LINUX calling convention not yet "
                                    "supported in create_cgcontext_child()");
            break;
        default:
            print_error(ERR_COMMON, "Encountered unknown target_call_conv in "
                                    "create_cgcontext_child()");
            break;
        }
        break;
    }
}

void code_gen_setup_func_call(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_setup_func_call_arch_x86_64(cg_ctx);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_setup_func_call()");
    }
}

void code_gen_ext_func_arg(CGContext *cg_ctx, RegDescriptor arg_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_ext_func_arg_arch_x86_64(cg_ctx, arg_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_ext_func_arg()");
    }
}

void code_gen_func_arg(CGContext *cg_ctx, RegDescriptor arg_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_func_arg_arch_x86_64(cg_ctx, arg_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "Encountered unknown target_fmt in code_gen_func_arg()");
    }
}

RegDescriptor code_gen_ext_func_call(CGContext *cg_ctx, const char *func_name) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_ext_func_call_arch_x86_64(cg_ctx, func_name);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_ext_func_call()");
    }
    return res_reg;
}

RegDescriptor code_gen_func_call(CGContext *cg_ctx, RegDescriptor func_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_func_call_arch_x86_64(cg_ctx, func_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "Encountered unknown target_fmt in code_gen_func_call()");
    }
    return res_reg;
}

void code_gen_cleanup(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_cleanup_arch_x86_64(cg_ctx);
        break;
    default:
        print_error(ERR_COMMON,
                    "Encountered unknown target_fmt in code_gen_cleanup()");
    }
}

RegDescriptor code_gen_get_global_addr(CGContext *cg_ctx, const char *sym) {

    RegDescriptor res_reg = reg_alloc(cg_ctx);
    code_gen_get_global_addr_into(cg_ctx, sym, res_reg);
    return res_reg;
}

RegDescriptor code_gen_get_local_addr(CGContext *cg_ctx, long offset) {

    RegDescriptor res_reg = reg_alloc(cg_ctx);
    code_gen_get_local_addr_into(cg_ctx, offset, res_reg);
    return res_reg;
}

void code_gen_get_global_addr_into(CGContext *cg_ctx, const char *sym,
                                   RegDescriptor target_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_get_global_into_arch_x86_64(cg_ctx, sym, target_reg);
        break;
    default:
        print_error(ERR_COMMON, "Encountered unknown target_fmt in "
                                "code_gen_get_global_addr_into()");
    }
}

void code_gen_get_local_addr_into(CGContext *cg_ctx, long offset,
                                  RegDescriptor target_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_get_local_addr_into_arch_x86_64(cg_ctx, offset, target_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_get_local_addr_into()");
    }
}

RegDescriptor code_gen_get_global(CGContext *cg_ctx, const char *sym) {

    RegDescriptor res_reg;
    code_gen_get_global_into(cg_ctx, sym, res_reg);
    return res_reg;
}

RegDescriptor code_gen_get_local(CGContext *cg_ctx, long offset) {

    RegDescriptor res_reg;
    code_gen_get_local_into(cg_ctx, offset, res_reg);
    return res_reg;
}

void code_gen_get_global_into(CGContext *cg_ctx, const char *sym,
                              RegDescriptor target_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_get_global_into_arch_x86_64(cg_ctx, sym, target_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_get_global_into()");
    }
}

void code_gen_get_local_into(CGContext *cg_ctx, long offset,
                             RegDescriptor target_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_get_local_into_arch_x86_64(cg_ctx, offset, target_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_get_local_into()");
    }
}

void code_gen_store_global(CGContext *cg_ctx, const char *sym,
                           RegDescriptor from_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_store_global_arch_x86_64(cg_ctx, sym, from_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "Encountered unknown target_fmt in code_gen_store_global()");
    }
}

void code_gen_store_local(CGContext *cg_ctx, long offset,
                          RegDescriptor from_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_store_local_arch_x86_64(cg_ctx, offset, from_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "Encountered unknown target_fmt in code_gen_store_local()");
    }
}

void code_gen_store(CGContext *cg_ctx, RegDescriptor from_reg,
                    RegDescriptor mem_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_store_arch_x86_64(cg_ctx, from_reg, mem_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "Encountered unknown target_fmt in code_gen_store()");
    }
}

void code_gen_add_imm(CGContext *cg_ctx, long data, RegDescriptor dest) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_add_imm_arch_x86_64(cg_ctx, data, dest);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_add_imm()");
    }
}

void code_gen_branch_if_zero(CGContext *cg_ctx, RegDescriptor reg_desc,
                             const char *jmp_label) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_branch_if_zero_arch_x86_64(cg_ctx, reg_desc, jmp_label);
        break;
    default:
        print_error(
            ERR_COMMON,
            "encountered unknown target_fmt in code_gen_branch_if_zero()");
    }
}

void code_gen_branch(CGContext *cg_ctx, const char *jmp_label) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_branch_arch_x86_64(cg_ctx, jmp_label);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_branch()");
    }
}

RegDescriptor code_gen_get_imm(CGContext *cg_ctx, long data) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_get_imm_arch_x86_64(cg_ctx, data);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_get_imm()");
    }
    return res_reg;
}

void code_gen_zero_out_reg(CGContext *cg_ctx, RegDescriptor reg_desc) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_zero_out_reg_arch_x86_64(cg_ctx, reg_desc);
        break;
    default:
        print_error(
            ERR_COMMON,
            "encountered unknown target_fmt in code_gen_zero_out_reg()");
    }
}

void code_gen_copy_reg(CGContext *cg_ctx, RegDescriptor src_reg,
                       RegDescriptor dest_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_copy_reg_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_copy_reg()");
    }
}

RegDescriptor code_gen_compare(CGContext *cg_ctx, ComparisonType comp_type,
                               RegDescriptor lhs_reg, RegDescriptor rhs_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg =
            code_gen_compare_arch_x86_64(cg_ctx, comp_type, lhs_reg, rhs_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_compare()");
    }
    return res_reg;
}

RegDescriptor code_gen_add(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_add_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_add()");
    }
    return res_reg;
}

RegDescriptor code_gen_sub(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_sub_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_sub()");
    }
    return res_reg;
}

RegDescriptor code_gen_mul(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_mul_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_mul()");
    }
    return res_reg;
}

RegDescriptor code_gen_div(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_div_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_div()");
    }
    return res_reg;
}

RegDescriptor code_gen_mod(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_mod_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_mod()");
    }
    return res_reg;
}

RegDescriptor code_gen_shift_left(CGContext *cg_ctx, RegDescriptor src_reg,
                                  RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_shift_left_arch_x86_64(cg_ctx, src_reg, dest_reg);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_shift_left()");
    }
    return res_reg;
}

RegDescriptor code_gen_shift_right_arithmetic(CGContext *cg_ctx,
                                              RegDescriptor src_reg,
                                              RegDescriptor dest_reg) {

    RegDescriptor res_reg;
    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        res_reg = code_gen_shift_right_arithmetic_arch_x86_64(cg_ctx, src_reg,
                                                              dest_reg);
        break;
    default:
        print_error(ERR_COMMON, "encountered unknown target_fmt in "
                                "code_gen_shift_right_arithmetic()");
    }
    return res_reg;
}

void code_gen_allocate_on_stack(CGContext *cg_ctx, long size) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_allocate_on_stack_arch_x86_64(cg_ctx, size);
        break;
    default:
        print_error(
            ERR_COMMON,
            "encountered unknown target_fmt in code_gen_allocate_on_stack()");
    }
}

void code_gen_func_header(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_func_header_arch_x86_64(cg_ctx);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_func_header()");
    }
}

void code_gen_func_footer(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_func_footer_arch_x86_64(cg_ctx);
        break;
    default:
        print_error(ERR_COMMON,
                    "encountered unknown target_fmt in code_gen_func_footer()");
    }
}

void code_gen_set_func_ret_val(CGContext *cg_ctx, RegDescriptor prev_reg) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_set_func_ret_val_arch_x86_64(cg_ctx, prev_reg);
        break;
    default:
        print_error(
            ERR_COMMON,
            "encountered unknown target_fmt in code_gen_set_func_ret_val()");
    }
}

void code_gen_set_entry_point(CGContext *cg_ctx) {

    switch (cg_ctx->target_fmt) {
    case TARGET_FMT_X86_64_GNU_AS:
        code_gen_set_entry_point_arch_x86_64(cg_ctx);
        break;
    default:
        print_error(
            ERR_COMMON,
            "encountered unknown target_fmt in code_gen_set_entry_point()");
    }
}
