#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct AstNode AstNode;
typedef struct Env Env;
typedef struct ParsingContext ParsingContext;

#define LABEL_ARR_SIZE 1024
#define SYM_ARR_SIZE 1024

#define FUNC_FOOTER_x86_64                                                     \
    "pop %rbp\n"                                                               \
    "ret\n"

#define FUNC_HEADER_x86_64                                                     \
    "push %rbp\n"                                                              \
    "mov %rsp, %rbp\n"                                                         \
    "sub $32, %rsp\n"

typedef struct Reg {
    char *reg_name;
    int reg_in_use;
    struct Reg *next_reg;
} Reg;

typedef int RegDescriptor;

typedef enum TargetType {
    TARGET_DEFAULT = 0,
    TARGET_x86_64_WIN,
} TargetType;

typedef struct CGContext {
    struct CGContext *parent_ctx;
    Env *local_env;
    long local_offset;
} CGContext;

// Create a new register with 'reg_name' and allocate memory for it.
Reg *reg_create_new(char *reg_name);

// Add a new register to the linked list of registers.
void reg_add_new(Reg **reg_head, char *reg_name);

// Free the register linked list.
void reg_free(Reg *reg_head);

// Look for a register in the linked list and find one that isn't in
// use and return its register descriptor descriptor.
RegDescriptor reg_alloc(Reg *reg_head);

// Get the name of the register based on the register descriptor.
char *get_reg_name(RegDescriptor reg_desc, Reg *reg_head);

// Free (Mark as not in use) the register that is in use.
void reg_dealloc(Reg *reg_head, RegDescriptor reg_desc);

// Generate labels for lambda functions.
char *gen_label();

char *map_sym_to_addr_win(CGContext *cg_ctx, AstNode *sym_node);

void target_x86_64_win_codegen_expr(Reg *reg_head, ParsingContext *context,
                                    AstNode *curr_expr, CGContext *cg_ctx,
                                    FILE *fptr_code);

void target_x86_64_win_codegen_func(Reg *reg_head, CGContext *cg_ctx,
                                    ParsingContext *context, char *func_name,
                                    AstNode *func, FILE *fptr_code);

void target_x86_64_win_codegen_prog(ParsingContext *context, AstNode *program,
                                    CGContext *cg_ctx, FILE *fptr_code);

void target_codegen(ParsingContext *context, AstNode *program,
                    char *output_file_path, TargetType type);

#ifdef __cplusplus
}
#endif

#endif /* __CODE_GEN_H__ */
