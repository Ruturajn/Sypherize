#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "parser.h"

#define LABEL_ARR_SIZE 1024
#define SYM_ARR_SIZE 1024

#define FUNC_FOOTER_x86_64                                                                         \
    "pop %rbp\n"                                                                                   \
    "ret\n"

#define FUNC_HEADER_x86_64                                                                         \
    "push %rbp\n"                                                                                  \
    "mov %rsp, %rbp\n"                                                                             \
    "sub $32, %rsp\n"

#define INIT_REGISTER(regs, register_desc, register_name)                                          \
    ((regs)[register_desc] =                                                                       \
         (Reg){.reg_name = (register_name), .reg_in_use = 0, .reg_desc = (register_desc)})

extern char codegen_verbose;

typedef int RegDescriptor;

typedef struct Reg {
    char *reg_name;
    int reg_in_use;
    RegDescriptor reg_desc;
} Reg;

typedef enum TargetFormat {
    TARGET_FMT_DEFAULT = 0,
    TARGET_FMT_x86_64_GNU_AS,
} TargetFormat;

typedef enum TargetCallingConvention {
    TARGET_CALL_CONV_LINUX = 0,
    TARGET_CALL_CONV_WIN,
} TargetCallingConvention;

typedef struct RegPool {
    Reg *regs;
    Reg **scratch_regs;
    int scratch_reg_cnt;
    int reg_cnt;
} RegPool;

typedef struct CGContext {
    struct CGContext *parent_ctx;
    Env *local_env;
    long local_offset;
    RegPool reg_pool;
    TargetCallingConvention target_call_conv;
    TargetFormat target_fmt;
} CGContext;

typedef enum Regs_X86_64 {
    REG_X86_64_RAX,
    REG_X86_64_RBX,
    REG_X86_64_RCX,
    REG_X86_64_RDX,
    REG_X86_64_R8,
    REG_X86_64_R9,
    REG_X86_64_R10,
    REG_X86_64_R11,
    REG_X86_64_R12,
    REG_X86_64_R13,
    REG_X86_64_R14,
    REG_X86_64_R15,
    REG_X86_64_RSI,
    REG_X86_64_RDI,
    REG_X86_64_RBP,
    REG_X86_64_RSP,
    REG_X86_64_RIP,
    REG_X86_64_COUNT,
} Regs_X86_64;

typedef enum ComparisonType {
    COMP_EQ,
    COMP_NE,
    COMP_LT,
    COMP_LE,
    COMP_GT,
    COMP_GE,

    COMP_COUNT,
} ComparisonType;

extern const char *comp_suffixes_x86_84[COMP_COUNT];

CGContext *create_codegen_context_gnu_as_win(CGContext *parent_ctx);

void free_codegen_context(CGContext *cg_ctx);

char is_valid_reg_desc(CGContext *cg_ctx, RegDescriptor reg_desc);

RegDescriptor reg_alloc(CGContext *cg_ctx);

// Get the name of the register based on the register descriptor.
const char *get_reg_name(CGContext *cg_ctx, RegDescriptor reg_desc);

// Free (Mark as not in use) the register that is in use.
void reg_dealloc(CGContext *cg_ctx, RegDescriptor reg_desc);

char *map_sym_to_addr_win(CGContext *cg_ctx, AstNode *sym_node);

void print_regs(CGContext *cg_ctx);

const char *get_byte_reg_name_x86_64_win(CGContext *cg_ctx, RegDescriptor reg_desc);

void target_x86_64_win_codegen_expr(ParsingContext *context, ParsingContext **ctx_next_child,
                                    AstNode *curr_expr, CGContext *cg_ctx, FILE *fptr_code);

void target_x86_64_gnu_as_codegen_func(CGContext *cg_ctx, ParsingContext *context,
                                       ParsingContext **ctx_next_child, char *func_name,
                                       AstNode *func, FILE *fptr_code);

void target_x86_64_win_codegen_prog(ParsingContext *context, AstNode *program, CGContext *cg_ctx,
                                    FILE *fptr_code);

void target_codegen(ParsingContext *context, AstNode *program, char *output_file_path,
                    TargetFormat type, TargetCallingConvention call_conv);

#ifdef __cplusplus
}
#endif

#endif /* __CODE_GEN_H__ */
