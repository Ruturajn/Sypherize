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

typedef int RegDescriptor;

typedef struct Reg {
    char *reg_name;
    int reg_in_use;
    RegDescriptor reg_desc;
} Reg;

typedef enum TargetType {
    TARGET_DEFAULT = 0,
    TARGET_x86_64_WIN,
} TargetType;

typedef struct RegPool {
    Reg *regs;
    int scratch_reg_cnt;
    int reg_cnt;
} RegPool;

typedef struct CGContext {
    struct CGContext *parent_ctx;
    Env *local_env;
    RegPool reg_pool;
    long local_offset;
} CGContext;

enum ScratchRegs_X86_64_WIN {
    // Scratch registers.
    REG_X86_64_WIN_RAX,
    REG_X86_64_WIN_RCX,
    REG_X86_64_WIN_RDX,
    REG_X86_64_WIN_R8,
    REG_X86_64_WIN_R9,
    REG_X86_64_WIN_R10,
    REG_X86_64_WIN_R11,

    // Non scratch registers.
    REG_X86_64_WIN_R12,
    REG_X86_64_WIN_R13,
    REG_X86_64_WIN_R14,
    REG_X86_64_WIN_R15,
    REG_X86_64_WIN_RBX,
    REG_X86_64_WIN_RSI,
    REG_X86_64_WIN_RDI,
    REG_X86_64_WIN_RBP,
    REG_X86_64_WIN_RSP,
    REG_X86_64_WIN_RIP,

    // Total count
    REG_X86_64_WIN_SCRATCH_COUNT = REG_X86_64_WIN_R11 + 1,
    REG_X86_64_WIN_COUNT = REG_X86_64_WIN_RIP + 1,
};

CGContext *create_codegen_context(CGContext *parent_ctx);

void free_codegen_context(CGContext *cg_ctx);

char is_valid_reg_desc(CGContext *cg_ctx, RegDescriptor reg_desc);

RegDescriptor reg_alloc(CGContext *cg_ctx);

// Get the name of the register based on the register descriptor.
char *get_reg_name(CGContext *cg_ctx, RegDescriptor reg_desc);

// Free (Mark as not in use) the register that is in use.
void reg_dealloc(CGContext *cg_ctx, RegDescriptor reg_desc);

// Generate labels for lambda functions.
char *gen_label();

char *map_sym_to_addr_win(CGContext *cg_ctx, AstNode *sym_node);

void print_regs(CGContext *cg_ctx);

void target_x86_64_win_codegen_expr(ParsingContext *context, ParsingContext **ctx_next_child,
                                    AstNode *curr_expr, CGContext *cg_ctx, FILE *fptr_code);

void target_x86_64_win_codegen_func(CGContext *cg_ctx, ParsingContext *context,
                                    ParsingContext **ctx_next_child, char *func_name, AstNode *func,
                                    FILE *fptr_code);

void target_x86_64_win_codegen_prog(ParsingContext *context, AstNode *program, CGContext *cg_ctx,
                                    FILE *fptr_code);

void target_codegen(ParsingContext *context, AstNode *program, char *output_file_path,
                    TargetType type);

#ifdef __cplusplus
}
#endif

#endif /* __CODE_GEN_H__ */
