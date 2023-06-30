#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#include "../inc/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TargetType {
    TARGET_DEFAULT = 0,
    TARGET_X86_64_AT_T_ASM,
} TargetType;

void choose_target(TargetType target, AstNode *program_node);

void target_x86_64_att_asm(AstNode *program_node);

#endif /* __CODE_GEN_H__ */
