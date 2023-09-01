#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "parser.h"

#define LABEL_ARR_SIZE 1024
#define SYM_ARR_SIZE 1024

#define FUNC_FOOTER_x86_64                                                     \
    "pop %rbp\n"                                                               \
    "ret\n"

#define FUNC_HEADER_x86_64                                                     \
    "push %rbp\n"                                                              \
    "mov %rsp, %rbp\n"                                                         \
    "sub $32, %rsp\n"

typedef enum TargetFormat {
    TARGET_FMT_X86_64_GNU_AS,
    TARGET_FMT_COUNT,

    TARGET_FMT_DEFAULT = TARGET_FMT_X86_64_GNU_AS,
} TargetFormat;

typedef enum TargetCallingConvention {
    TARGET_CALL_CONV_WIN,
    TARGET_CALL_CONV_LINUX,
    TARGET_CALL_CONV_COUNT,

    TARGET_CALL_CONV_DEFAULT = TARGET_CALL_CONV_WIN,
} TargetCallingConvention;

typedef enum TargetAssemblyDialect {
    TARGET_ASM_DIALECT_ATT,
    TARGET_ASM_DIALECT_INTEL,
    TARGET_ASM_DIALECT_COUNT,

    TARGET_ASM_DIALECT_DEFAULT = TARGET_ASM_DIALECT_ATT,
} TargetAssemblyDialect;

typedef enum ComparisonType {
    COMP_EQ,
    COMP_NE,
    COMP_LT,
    COMP_LE,
    COMP_GT,
    COMP_GE,

    COMP_COUNT,
} ComparisonType;

extern char codegen_verbose;

typedef int RegDescriptor;

typedef struct Reg {
    int reg_in_use;
    RegDescriptor reg_desc;
} Reg;

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
    FILE *fptr_code;
    void *arch_data;
    TargetCallingConvention target_call_conv;
    TargetFormat target_fmt;
    TargetAssemblyDialect target_asm_dialect;
} CGContext;

extern const char *comp_suffixes_x86_84[COMP_COUNT];

char is_valid_reg_desc(CGContext *cg_ctx, RegDescriptor reg_desc);

RegDescriptor reg_alloc(CGContext *cg_ctx);

// Free (Mark as not in use) the register that is in use.
void reg_dealloc(CGContext *cg_ctx, RegDescriptor reg_desc);

void print_regs(CGContext *cg_ctx);

#ifdef __cplusplus
}
#endif

#endif /* __CODE_GEN_H__ */
