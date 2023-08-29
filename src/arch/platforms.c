#include "../../inc/arch/platforms.h"
#include "../../inc/arch/x86_64/code_gen_x86_64.h"

CGContext *create_cgcontext(TargetFormat fmt, TargetCallingConvention call_conv,
                            TargetAssemblyDialect dialect, FILE *fptr) {

    CGContext *cg_ctx = NULL;

    if (call_conv == TARGET_CALL_CONV_WIN)
        cg_ctx = create_codegen_context_gnu_as_win(NULL);
    else if (call_conv == TARGET_CALL_CONV_LINUX) {
        print_error(ERR_ARGS, "LINUX calling convention not yet supported :(");
    } else {
        print_error(ERR_ARGS, "Encountered invalid calling convention");
    }

    return cg_ctx;
}

CGContext *create_cgcontext_child(CGContext *parent_ctx);

void free_cgcontext_child(CGContext *parent_ctx);

void code_gen_setup_func_call(CGContext *cg_ctx);

void code_gen_ext_func_arg(CGContext *cg_ctx, RegDescriptor arg_reg);

void code_gen_func_arg(CGContext *cg_ctx, RegDescriptor arg_reg);

RegDescriptor code_gen_ext_func_call(CGContext *cg_ctx, const char *func_name);

RegDescriptor code_gen_func_call(CGContext *cg_ctx, RegDescriptor func_reg);

void code_gen_cleanup(CGContext *cg_ctx);

RegDescriptor code_gen_get_global_addr(CGContext *cg_ctx, const char *sym);

RegDescriptor code_gen_get_local_addr(CGContext *cg_ctx, long offset);

void code_gen_get_global_addr_into(CGContext *cg_ctx, const char *sym,
                                   RegDescriptor target_reg);

void code_gen_get_local_addr_into(CGContext *cg_ctx, long offset,
                                  RegDescriptor target_reg);

RegDescriptor code_gen_get_global(CGContext *cg_ctx, const char *sym);

RegDescriptor code_gen_get_local(CGContext *cg_ctx, long offset);

void code_gen_get_global_into(CGContext *cg_ctx, const char *sym,
                              RegDescriptor target_reg);

void code_gen_get_local_into(CGContext *cg_ctx, long offset,
                             RegDescriptor target_reg);

void code_gen_store_global(CGContext *cg_ctx, const char *sym,
                           RegDescriptor from_reg);

void code_gen_store_local(CGContext *cg_ctx, long offset,
                          RegDescriptor from_reg);

void code_gen_store(CGContext *cg_ctx, RegDescriptor from_reg,
                    RegDescriptor mem_reg);

void code_gen_add_imm(CGContext *cg_ctx, long data, RegDescriptor dest);

void code_gen_branch_if_zero(CGContext *cg_ctx, RegDescriptor reg_desc,
                             const char *jmp_label);

void code_gen_branch(CGContext *cg_ctx, const char *jmp_label);

RegDescriptor code_gen_get_imm(CGContext *cg_ctx, long data);

void code_gen_zero_out_reg(CGContext *cg_ctx, RegDescriptor reg_desc);

void code_gen_copy_reg(CGContext *cg_ctx, RegDescriptor src_reg,
                       RegDescriptor dest_reg);

RegDescriptor code_gen_compare(CGContext *cg_ctx, ComparisonType comp_type,
                               RegDescriptor lhs_reg, RegDescriptor rhs_reg);

RegDescriptor code_gen_add(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg);

RegDescriptor code_gen_sub(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg);

RegDescriptor code_gen_mul(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg);

RegDescriptor code_gen_div(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg);

RegDescriptor code_gen_mod(CGContext *cg_ctx, RegDescriptor src_reg,
                           RegDescriptor dest_reg);

RegDescriptor code_gen_shift_left(CGContext *cg_ctx, RegDescriptor src_reg,
                                  RegDescriptor dest_reg);

RegDescriptor code_gen_shift_right_arithmetic(CGContext *cg_ctx,
                                              RegDescriptor src_reg,
                                              RegDescriptor dest_reg);

void code_gen_allocate_on_stack(CGContext *cg_ctx, long size);

void code_gen_func_header(CGContext *cg_ctx);

void code_gen_func_footer(CGContext *cg_ctx);

void code_gen_set_func_ret_val(CGContext *cg_ctx);

void code_gen_set_entry_point(CGContext *cg_ctx);
