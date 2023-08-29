#ifndef __CODE_GEN_X86_64_H__
#define __CODE_GEN_X86_64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../code_gen.h"

#define FOR_ALL_X86_64_REGS(FUNC)                                              \
    FUNC(RAX, "rax", "eax", "ax", "al")                                        \
    FUNC(RBX, "rbx", "ebx", "bx", "bl")                                        \
    FUNC(RCX, "rcx", "ecx", "cx", "cl")                                        \
    FUNC(RDX, "rdx", "edx", "dx", "dl")                                        \
    FUNC(R8, "r8", "r8d", "r8w", "r8b")                                        \
    FUNC(R9, "r9", "r9d", "r9w", "r9b")                                        \
    FUNC(R10, "r10", "r10d", "r10w", "r10b")                                   \
    FUNC(R11, "r11", "r11d", "r11w", "r11b")                                   \
    FUNC(R12, "r12", "r12d", "r12w", "r12b")                                   \
    FUNC(R13, "r13", "r13d", "r13w", "r13b")                                   \
    FUNC(R14, "r14", "r14d", "r14w", "r14b")                                   \
    FUNC(R15, "r15", "r15d", "r15w", "r15b")                                   \
    FUNC(RSI, "rsi", "esi", "si", "sil")                                       \
    FUNC(RDI, "rdi", "edi", "di", "dil")                                       \
    FUNC(RBP, "rbp", "ebp", "bp", "bpl")                                       \
    FUNC(RSP, "rsp", "esp", "sp", "spl")                                       \
    FUNC(RIP, "rip", "eip", "ip", "ipl")

CGContext *create_codegen_context_gnu_as_win(CGContext *parent_ctx);

void free_codegen_context(CGContext *cg_ctx);

CGContext *create_cgcontext_arch_x86_64(TargetFormat fmt,
                                        TargetCallingConvention call_conv,
                                        TargetAssemblyDialect dialect,
                                        FILE *fptr);

CGContext *create_cgcontext_child_arch_x86_64(CGContext *parent_ctx);

void free_cgcontext_child_arch_x86_64(CGContext *parent_ctx);

void code_gen_setup_func_call_arch_x86_64(CGContext *cg_ctx);

void code_gen_ext_func_arg_arch_x86_64(CGContext *cg_ctx,
                                       RegDescriptor arg_reg);

void code_gen_func_arg_arch_x86_64(CGContext *cg_ctx, RegDescriptor arg_reg);

RegDescriptor code_gen_ext_func_call_arch_x86_64(CGContext *cg_ctx,
                                                 const char *func_name);

RegDescriptor code_gen_func_call_arch_x86_64(CGContext *cg_ctx,
                                             RegDescriptor func_reg);

void code_gen_cleanup_arch_x86_64(CGContext *cg_ctx);

RegDescriptor code_gen_get_global_addr_arch_x86_64(CGContext *cg_ctx,
                                                   const char *sym);

RegDescriptor code_gen_get_local_addr_arch_x86_64(CGContext *cg_ctx,
                                                  long offset);

void code_gen_get_global_addr_into_arch_x86_64(CGContext *cg_ctx,
                                               const char *sym,
                                               RegDescriptor target_reg);

void code_gen_get_local_addr_into_arch_x86_64(CGContext *cg_ctx, long offset,
                                              RegDescriptor target_reg);

RegDescriptor code_gen_get_global_arch_x86_64(CGContext *cg_ctx,
                                              const char *sym);

RegDescriptor code_gen_get_local_arch_x86_64(CGContext *cg_ctx, long offset);

void code_gen_get_global_into_arch_x86_64(CGContext *cg_ctx, const char *sym,
                                          RegDescriptor target_reg);

void code_gen_get_local_into_arch_x86_64(CGContext *cg_ctx, long offset,
                                         RegDescriptor target_reg);

void code_gen_store_global_arch_x86_64(CGContext *cg_ctx, const char *sym,
                                       RegDescriptor from_reg);

void code_gen_store_local_arch_x86_64(CGContext *cg_ctx, long offset,
                                      RegDescriptor from_reg);

void code_gen_store_arch_x86_64(CGContext *cg_ctx, RegDescriptor from_reg,
                                RegDescriptor mem_reg);

void code_gen_add_imm_arch_x86_64(CGContext *cg_ctx, long data,
                                  RegDescriptor dest);

void code_gen_branch_if_zero_arch_x86_64(CGContext *cg_ctx,
                                         RegDescriptor reg_desc,
                                         const char *jmp_label);

void code_gen_branch_arch_x86_64(CGContext *cg_ctx, const char *jmp_label);

RegDescriptor code_gen_get_imm_arch_x86_64(CGContext *cg_ctx, long data);

void code_gen_zero_out_reg_arch_x86_64(CGContext *cg_ctx,
                                       RegDescriptor reg_desc);

void code_gen_copy_reg_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                   RegDescriptor dest_reg);

RegDescriptor code_gen_compare_arch_x86_64(CGContext *cg_ctx,
                                           ComparisonType comp_type,
                                           RegDescriptor lhs_reg,
                                           RegDescriptor rhs_reg);

RegDescriptor code_gen_add_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg);

RegDescriptor code_gen_sub_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg);

RegDescriptor code_gen_mul_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg);

RegDescriptor code_gen_div_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg);

RegDescriptor code_gen_mod_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg);

RegDescriptor code_gen_shift_left_arch_x86_64(CGContext *cg_ctx,
                                              RegDescriptor src_reg,
                                              RegDescriptor dest_reg);

RegDescriptor code_gen_shift_right_arithmetic_arch_x86_64(
    CGContext *cg_ctx, RegDescriptor src_reg, RegDescriptor dest_reg);

void code_gen_allocate_on_stack_arch_x86_64(CGContext *cg_ctx, long size);

void code_gen_func_header_arch_x86_64(CGContext *cg_ctx);

void code_gen_func_footer_arch_x86_64(CGContext *cg_ctx);

void code_gen_set_func_ret_val_arch_x86_64(CGContext *cg_ctx);

void code_gen_set_entry_point_arch_x86_64(CGContext *cg_ctx);

#ifdef __cplusplus
}
#endif

#endif /* __CODE_GEN_X86_64_H__ */
