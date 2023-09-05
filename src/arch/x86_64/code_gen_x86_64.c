#include "../../../inc/arch/x86_64/code_gen_x86_64.h"
#include "../../../inc/code_gen.h"
#include "../../../inc/env_funcs.h"
#include <inttypes.h>

#define DEFINE_REG_ENUM(name, ...) REG_X86_64_##name,
#define REGITER_NAME_64(ident, name, ...) name,
#define REGITER_NAME_32(ident, name, name_32, ...) name_32,
#define REGITER_NAME_16(ident, name, name_32, name_16, ...) name_16,
#define REGITER_NAME_8(ident, name, name_32, name_16, name_8, ...) name_8,

#define INIT_REGISTER(ident, ...)                                              \
    ((registers)[REG_X86_64_##ident] =                                         \
         (Reg){.reg_in_use = 0, .reg_desc = (REG_X86_64_##ident)});

#define DEFINE_REG_NAME_LOOKUP_FUNC(name, bits)                                \
    static const char *name(RegDescriptor reg_desc) {                          \
        static const char *reg_names[] = {                                     \
            FOR_ALL_X86_64_REGS(REGITER_NAME_##bits)};                         \
        if (reg_desc < 0 || reg_desc >= REG_X86_64_COUNT) {                    \
            print_error(ERR_COMMON, "Invalid register descriptor: `%d`",       \
                        reg_desc);                                             \
        }                                                                      \
        return reg_names[reg_desc];                                            \
    }

typedef enum Regs_X86_64 {
    FOR_ALL_X86_64_REGS(DEFINE_REG_ENUM) REG_X86_64_COUNT,
} Regs_X86_64;

DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name, 64)
// Following functions are commented out due to unused errors.
// They will need to be defined again, when needed.
// DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_32, 32)
// DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_16, 16)
DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_8, 8)

#undef REGISTER_NAME_64
#undef REGISTER_NAME_32
#undef REGISTER_NAME_16
#undef REGISTER_NAME_8

#undef DEFINE_REG_ENUM
#undef DEFINE_REG_NAME_LOOKUP_FUNC

typedef enum JumpType_X86_64 {
    JMP_TYPE_A,
    JMP_TYPE_AE,
    JMP_TYPE_B,
    JMP_TYPE_BE,
    JMP_TYPE_C,
    JMP_TYPE_E,
    JMP_TYPE_Z,
    JMP_TYPE_G,
    JMP_TYPE_GE,
    JMP_TYPE_L,
    JMP_TYPE_LE,
    JMP_TYPE_NA,
    JMP_TYPE_NAE,
    JMP_TYPE_NB,
    JMP_TYPE_NBE,
    JMP_TYPE_NC,
    JMP_TYPE_NE,
    JMP_TYPE_NG,
    JMP_TYPE_NGE,
    JMP_TYPE_NL,
    JMP_TYPE_NLE,
    JMP_TYPE_NO,
    JMP_TYPE_NP,
    JMP_TYPE_NS,
    JMP_TYPE_NZ,
    JMP_TYPE_O,
    JMP_TYPE_P,
    JMP_TYPE_PE,
    JMP_TYPE_PO,
    JMP_TYPE_S,

    JMP_TYPE_COUNT,
} JumpType_X86_64;

static const char *jmp_type_x86_64_strings[JMP_TYPE_COUNT] = {
    "a",   "ae", "b",   "be", "c",   "e",  "z",  "g",  "ge",  "l",
    "le",  "na", "nae", "nb", "nbe", "nc", "ne", "ng", "nge", "nl",
    "nle", "no", "np",  "ns", "nz",  "o",  "p",  "pe", "po",  "s"};

typedef enum Instructions_X86_64 {
    INST_X86_64_ADD,
    INST_X86_64_SUB,
    INST_X86_64_MUL,
    INST_X86_64_IMUL,
    INST_X86_64_DIV,
    INST_X86_64_IDIV,
    INST_X86_64_PUSH,
    INST_X86_64_POP,
    INST_X86_64_RET,
    INST_X86_64_MOV,
    INST_X86_64_LEA,
    INST_X86_64_XOR,
    INST_X86_64_JMP,
    INST_X86_64_CALL,
    INST_X86_64_CMP,
    INST_X86_64_TEST,
    INST_X86_64_CQO,
    INST_X86_64_SETCC,
    INST_X86_64_SAL,
    INST_X86_64_SHL = INST_X86_64_SAL,
    INST_X86_64_SAR,
    INST_X86_64_SHR,
    INST_X86_64_JCC,
    INST_X86_64_XCHG,

    INST_X86_64_COUNT,
} Instructions_X86_64;

typedef enum Instructions_Type_X86_64 {
    OPERAND_TYPE_REG,
    OPERAND_TYPE_IMM,
    OPERAND_TYPE_MEM,
    OPERAND_TYPE_SYM,
    OPERAND_TYPE_IMM_TO_MEM,
    OPERAND_TYPE_IMM_TO_REG,
    OPERAND_TYPE_MEM_TO_REG,
    OPERAND_TYPE_SYM_TO_REG,
    OPERAND_TYPE_REG_TO_SYM,
    OPERAND_TYPE_REG_TO_REG,
    OPERAND_TYPE_REG_TO_MEM,
} Instructions_Type_X86_64;

const char *comp_suffixes_x86_84[COMP_COUNT] = {
    "e", "ne", "l", "le", "g", "ge",
};

const char *inst_mnemonic_x86_64(CGContext *cg_ctx, Instructions_X86_64 inst,
                                 TargetFormat target) {
    switch (target) {
    default:
        break;
    }
    switch (inst) {
    default:
        break;
    case INST_X86_64_ADD:
        return "add";
    case INST_X86_64_SUB:
        return "sub";
    case INST_X86_64_MUL:
        return "mul";
    case INST_X86_64_IMUL:
        return "imul";
    case INST_X86_64_DIV:
        return "div";
    case INST_X86_64_IDIV:
        return "idiv";
    case INST_X86_64_PUSH:
        return "push";
    case INST_X86_64_POP:
        return "pop";
    case INST_X86_64_RET:
        return "ret";
    case INST_X86_64_MOV:
        return "mov";
    case INST_X86_64_LEA:
        return "lea";
    case INST_X86_64_XOR:
        return "xor";
    case INST_X86_64_JMP:
        return "jmp";
    case INST_X86_64_CALL:
        return "call";
    case INST_X86_64_CMP:
        return "cmp";
    case INST_X86_64_TEST:
        return "test";
    case INST_X86_64_SETCC:
        return "set";
    case INST_X86_64_SAL:
        return "sal";
    case INST_X86_64_SAR:
        return "sar";
    case INST_X86_64_SHR:
        return "shr";
    case INST_X86_64_JCC:
        return "j";
    case INST_X86_64_XCHG:
        return "xchg";
    }
    switch (cg_ctx->target_asm_dialect) {
    default:
        break;
    case TARGET_ASM_DIALECT_ATT:
        switch (inst) {
        default:
            break;
        case INST_X86_64_CQO:
            return "cqto";
        }
        break;

    case TARGET_ASM_DIALECT_INTEL:
        switch (inst) {
        default:
            break;
        case INST_X86_64_CQO:
            return "cqo";
        }
        break;
    }
    print_error(
        ERR_COMMON,
        "Unable to convert Instructions_X86_64 to corresponding mnemonic");
    return "";
}

static void file_emit_x86_64_imm_to_mem(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // immediate value, memory offset, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    int64_t mem_offset = va_arg(operands, int64_t);
    RegDescriptor mem_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s $%" PRId64 ", %" PRId64 "(%%%s)\n",
                mnemonic, imm_val, mem_offset, get_reg_name(mem_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s [%s + %" PRId64 "], %" PRId64 "\n",
                mnemonic, get_reg_name(mem_reg), mem_offset, imm_val);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `imm_to_mem`");
        break;
    }
}

static void file_emit_x86_64_imm_to_reg(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // immediate value, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s $%" PRId64 ", %%%s\n", mnemonic, imm_val,
                get_reg_name(dest_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s, %" PRId64 "\n", mnemonic,
                get_reg_name(dest_reg), imm_val);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `imm_to_reg`");
        break;
    }
}

static void file_emit_x86_64_mem_to_reg(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // memory offset, source, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %" PRId64 "(%%%s), %%%s\n", mnemonic,
                imm_val, get_reg_name(src_reg), get_reg_name(dest_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s, [%s + %" PRId64 "]\n", mnemonic,
                get_reg_name(dest_reg), get_reg_name(src_reg), imm_val);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `mem_to_reg`");
        break;
    }
}

static void file_emit_x86_64_sym_to_reg(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // name, source, destination.
    const char *sym = va_arg(operands, const char *);
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %s(%%%s), %%%s\n", mnemonic, sym,
                get_reg_name(src_reg), get_reg_name(dest_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s, [%s + %s]\n", mnemonic,
                get_reg_name(dest_reg), get_reg_name(src_reg), sym);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `sym_to_reg`");
        break;
    }
}

static void file_emit_x86_64_reg_to_reg(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // source, destination.
    RegDescriptor reg_src = va_arg(operands, RegDescriptor);
    RegDescriptor reg_dest = va_arg(operands, RegDescriptor);

    // Don't emit `mov` if the source and destination are the same.
    if (reg_src == reg_dest && strcmp(mnemonic, "mov") == 0)
        return;

    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %%%s, %%%s\n", mnemonic,
                get_reg_name(reg_src), get_reg_name(reg_dest));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s, %s\n", mnemonic,
                get_reg_name(reg_src), get_reg_name(reg_src));
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg_to_reg`");
        break;
    }
}

static void file_emit_x86_64_reg_to_mem(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // source, memory offset, destination.
    RegDescriptor reg_src = va_arg(operands, RegDescriptor);
    int64_t mem_offset = va_arg(operands, int64_t);
    RegDescriptor reg_dest = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %%%s, %" PRId64 "(%%%s)\n", mnemonic,
                get_reg_name(reg_src), mem_offset, get_reg_name(reg_dest));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s [%s + %" PRId64 "], %s\n", mnemonic,
                get_reg_name(reg_dest), mem_offset, get_reg_name(reg_src));
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg_to_mem`");
        break;
    }
}

static void file_emit_x86_64_reg_to_sym(CGContext *cg_ctx, const char *mnemonic,
                                        va_list operands) {

    // name, source, destination.
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    const char *sym = va_arg(operands, const char *);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %%%s, %s(%%%s)\n", mnemonic,
                get_reg_name(src_reg), sym, get_reg_name(dest_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s [%s + %s], %s\n", mnemonic,
                get_reg_name(dest_reg), get_reg_name(src_reg), sym);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `sym_to_reg`");
        break;
    }
}

static void file_emit_x86_64_reg(CGContext *cg_ctx, const char *mnemonic,
                                 va_list operands) {
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %%%s\n", mnemonic,
                get_reg_name(src_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s\n", mnemonic, get_reg_name(src_reg));
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg`");
        break;
    }
}

static void file_emit_x86_64_mem(CGContext *cg_ctx, const char *mnemonic,
                                 va_list operands) {
    int64_t mem_offset = va_arg(operands, int64_t);
    RegDescriptor reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s %" PRId64 "(%%%s)\n", mnemonic,
                mem_offset, get_reg_name(reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s [%s + %" PRId64 "]\n", mnemonic,
                get_reg_name(reg), mem_offset);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg`");
        break;
    }
}

static void file_emit_x86_64_imm(CGContext *cg_ctx, const char *mnemonic,
                                 va_list operands) {
    int64_t imm_val = va_arg(operands, int64_t);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s $%" PRId64 "\n", mnemonic, imm_val);
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %" PRId64 "\n", mnemonic, imm_val);
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg`");
        break;
    }
}

static void file_emit_x86_64_indirect_branch(CGContext *cg_ctx,
                                             const char *mnemonic,
                                             va_list operands) {
    RegDescriptor addr_reg = va_arg(operands, RegDescriptor);
    switch (cg_ctx->target_asm_dialect) {
    case TARGET_ASM_DIALECT_ATT:
        fprintf(cg_ctx->fptr_code, "%s *%%%s\n", mnemonic,
                get_reg_name(addr_reg));
        break;
    case TARGET_ASM_DIALECT_INTEL:
        fprintf(cg_ctx->fptr_code, "%s %s\n", mnemonic, get_reg_name(addr_reg));
        break;
    default:
        print_error(ERR_COMMON,
                    "Unrecognized ASM dialect encountered for `reg`");
        break;
    }
}

static void file_emit_x86_64(CGContext *cg_ctx, Instructions_X86_64 inst, ...) {
    va_list operands;
    va_start(operands, inst);

    if (cg_ctx == NULL)
        print_error(ERR_COMMON, "Encountered NULL code gen context");

    const char *mnemonic =
        inst_mnemonic_x86_64(cg_ctx, inst, cg_ctx->target_fmt);

    switch (inst) {

    default:
        print_error(ERR_COMMON,
                    "Unhandled instruction in file_emit_x86_64_inst()");
        break;

    case INST_X86_64_ADD:
    case INST_X86_64_SUB:
    case INST_X86_64_TEST:
    case INST_X86_64_XOR:
    case INST_X86_64_CMP:
    case INST_X86_64_MOV:;
        Instructions_Type_X86_64 inst_type =
            va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(
                ERR_COMMON,
                "Invalid instruction type for : `add/sub/test/xor/cmp/mov`");
            break;
        case OPERAND_TYPE_IMM_TO_REG:
            file_emit_x86_64_imm_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_IMM_TO_MEM:
            file_emit_x86_64_imm_to_mem(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_MEM_TO_REG:
            file_emit_x86_64_mem_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG_TO_MEM:
            file_emit_x86_64_reg_to_mem(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG_TO_REG:
            file_emit_x86_64_reg_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG_TO_SYM:
            file_emit_x86_64_reg_to_sym(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_SYM_TO_REG:
            file_emit_x86_64_sym_to_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_MUL:
        break;

    case INST_X86_64_IMUL:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `imul`");
            break;
        case OPERAND_TYPE_MEM_TO_REG:
            file_emit_x86_64_mem_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG_TO_REG:
            file_emit_x86_64_reg_to_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_DIV:
        break;

    case INST_X86_64_IDIV:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `idiv`");
            break;
        case OPERAND_TYPE_MEM:
            file_emit_x86_64_mem(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG:
            file_emit_x86_64_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_PUSH:;
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `push`");
            break;
        case OPERAND_TYPE_MEM:
            file_emit_x86_64_mem(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG:
            file_emit_x86_64_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_IMM:
            file_emit_x86_64_imm(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_POP:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `push`");
            break;
        case OPERAND_TYPE_MEM:
            file_emit_x86_64_mem(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG:
            file_emit_x86_64_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_RET:
    case INST_X86_64_CQO:
        fprintf(cg_ctx->fptr_code, "%s\n", mnemonic);
        break;

    case INST_X86_64_LEA:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `lea`");
            break;
        case OPERAND_TYPE_MEM_TO_REG:
            file_emit_x86_64_mem_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_SYM_TO_REG:
            file_emit_x86_64_sym_to_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;

    case INST_X86_64_JMP:
    case INST_X86_64_CALL:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON,
                        "Invalid instruction type for : `jmp/call`");
            break;
        case OPERAND_TYPE_REG:
            file_emit_x86_64_indirect_branch(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_SYM:;
            const char *sym = va_arg(operands, const char *);
            switch (cg_ctx->target_asm_dialect) {
            case TARGET_ASM_DIALECT_ATT:
            case TARGET_ASM_DIALECT_INTEL:
                fprintf(cg_ctx->fptr_code, "%s %s\n", mnemonic, sym);
                break;
            default:
                print_error(ERR_COMMON,
                            "Invalid ASM dialect for instruction : `jmp/call`");
                break;
            }
            break;
        }
        break;

    case INST_X86_64_SETCC:;
        ComparisonType comp_type = va_arg(operands, ComparisonType);
        RegDescriptor reg_desc = va_arg(operands, RegDescriptor);
        switch (cg_ctx->target_asm_dialect) {
        default:
            print_error(ERR_COMMON,
                        "Invalid ASM dialect for instruction : `cqo`");
            break;
        case TARGET_ASM_DIALECT_ATT:
            fprintf(cg_ctx->fptr_code, "%s%s %%%s\n", mnemonic,
                    comp_suffixes_x86_84[comp_type], get_reg_name_8(reg_desc));
            break;
        case TARGET_ASM_DIALECT_INTEL:
            fprintf(cg_ctx->fptr_code, "%s%s %s\n", mnemonic,
                    comp_suffixes_x86_84[comp_type], get_reg_name_8(reg_desc));
            break;
        }
        break;

    case INST_X86_64_SAL:
    case INST_X86_64_SAR:
    case INST_X86_64_SHR:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON,
                        "Invalid instruction type for : `sal/shr/sar`");
            break;
        case OPERAND_TYPE_IMM_TO_REG:
            file_emit_x86_64_imm_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG:;
            RegDescriptor shift_reg = va_arg(operands, RegDescriptor);
            switch (cg_ctx->target_asm_dialect) {
            default:
                print_error(ERR_COMMON, "Unrecognized format for instruction : "
                                        "`sal/shr/sar`");
                break;
            case TARGET_ASM_DIALECT_ATT:
                fprintf(cg_ctx->fptr_code, "%s %%%s, %%%s", mnemonic,
                        get_reg_name_8(REG_X86_64_RCX),
                        get_reg_name(shift_reg));
                break;
            case TARGET_ASM_DIALECT_INTEL:
                fprintf(cg_ctx->fptr_code, "%s %s, %s", mnemonic,
                        get_reg_name(shift_reg),
                        get_reg_name_8(REG_X86_64_RCX));
                break;
            }
            break;
        }
        break;

    case INST_X86_64_JCC:;
        JumpType_X86_64 jmp_type = va_arg(operands, JumpType_X86_64);
        if (jmp_type > JMP_TYPE_COUNT)
            print_error(ERR_COMMON, "Invalid JCC type operation");
        const char *jmp_label = va_arg(operands, const char *);
        switch (cg_ctx->target_asm_dialect) {
        case TARGET_ASM_DIALECT_ATT:
        case TARGET_ASM_DIALECT_INTEL:
            fprintf(cg_ctx->fptr_code, "%s%s %s\n", mnemonic,
                    jmp_type_x86_64_strings[jmp_type], jmp_label);
            break;
        default:
            print_error(ERR_COMMON,
                        "Invalid ASM dialect for instruction : `jmp/call`");
            break;
        }
        break;

    case INST_X86_64_XCHG:
        inst_type = va_arg(operands, Instructions_Type_X86_64);
        switch (inst_type) {
        default:
            print_error(ERR_COMMON, "Invalid instruction type for : `xchg`");
            break;
        case OPERAND_TYPE_MEM_TO_REG:
            file_emit_x86_64_mem_to_reg(cg_ctx, mnemonic, operands);
            break;
        case OPERAND_TYPE_REG_TO_REG:
            file_emit_x86_64_reg_to_reg(cg_ctx, mnemonic, operands);
            break;
        }
        break;
    }
    va_end(operands);
}

typedef struct ArchData {
    enum {
        FUNC_CALL_NONE,
        FUNC_CALL_EXTERNAL,
        FUNC_CALL_INTERNAL,
    } func_call;
    long num_call_args;
    char is_rax_in_use;
} ArchData;

CGContext *create_cgcontext_gnu_as_win(CGContext *parent_ctx) {
    RegPool pool;

    // "The x64 ABI considers the registers RAX, RCX, RDX, R8, R9, R10, R11, and
    // XMM0-XMM5 volatile."
    // "The x64 ABI considers registers RBX, RBP, RDI, RSI, RSP, R12, R13, R14,
    // R15, and XMM6-XMM15 nonvolatile" Initialize the registers only when we
    // are creating the global context.
    if (parent_ctx == NULL) {
        Reg *registers = calloc(REG_X86_64_COUNT, sizeof(Reg));
        CHECK_NULL(registers, "Unable to allocate memory for registers array",
                   NULL);
        FOR_ALL_X86_64_REGS(INIT_REGISTER);

        int num_scratch_regs = 7;
        Reg **scratch_registers = calloc(num_scratch_regs, sizeof(Reg *));
        CHECK_NULL(registers,
                   "Unable to allocate memory for scratch registers array",
                   NULL);
        scratch_registers[0] = registers + REG_X86_64_RAX;
        scratch_registers[1] = registers + REG_X86_64_RCX;
        scratch_registers[2] = registers + REG_X86_64_RDX;
        scratch_registers[3] = registers + REG_X86_64_R8;
        scratch_registers[4] = registers + REG_X86_64_R9;
        scratch_registers[5] = registers + REG_X86_64_R10;
        scratch_registers[6] = registers + REG_X86_64_R11;

        pool.regs = registers;
        pool.scratch_regs = scratch_registers,
        pool.scratch_reg_cnt = num_scratch_regs,
        pool.reg_cnt = REG_X86_64_COUNT;

    } else {
        // If parent exists use it's register pool.
        pool = parent_ctx->reg_pool;
    }

    CGContext *new_ctx = calloc(1, sizeof(CGContext));
    CHECK_NULL(new_ctx, "Unable to allocate memory for new codegen context",
               NULL);
    new_ctx->parent_ctx = parent_ctx;
    new_ctx->local_env = create_env(NULL);
    new_ctx->local_offset = -32;
    new_ctx->reg_pool = pool;

    if (parent_ctx == NULL) {
        new_ctx->target_fmt = TARGET_FMT_X86_64_GNU_AS;
        new_ctx->target_call_conv = TARGET_CALL_CONV_WIN;
        new_ctx->target_asm_dialect = TARGET_ASM_DIALECT_ATT;
        ArchData *new_arch_data = calloc(1, sizeof(ArchData));
        CHECK_NULL(new_ctx, "Unable to allocate memory for new ArchData", NULL);
        new_ctx->arch_data = new_arch_data;
    } else {
        new_ctx->target_fmt = parent_ctx->target_fmt;
        new_ctx->target_call_conv = parent_ctx->target_call_conv;
        new_ctx->target_asm_dialect = parent_ctx->target_asm_dialect;
        new_ctx->fptr_code = parent_ctx->fptr_code;
        new_ctx->arch_data = parent_ctx->arch_data;
    }

    return new_ctx;
}

void free_cgcontext_gnu_as_win(CGContext *cg_ctx) {
    if (cg_ctx->parent_ctx == NULL) {
        free(cg_ctx->reg_pool.regs);
        free(cg_ctx->reg_pool.scratch_regs);
        free(cg_ctx->arch_data);
    }
    // Free environments.
    free(cg_ctx);
}

static RegDescriptor copy_ret_val_from_rax(CGContext *cg_ctx) {
    ArchData *arch_data = cg_ctx->arch_data;
    if (arch_data->func_call == FUNC_CALL_NONE)
        print_error(ERR_COMMON, "Return value from RAX can only be copied on"
                                "non NONE function call type");

    if (arch_data->is_rax_in_use) {
        RegDescriptor res_reg = reg_alloc(cg_ctx);
        file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                         REG_X86_64_RAX, res_reg);
        return res_reg;
    }

    return REG_X86_64_RAX;
}

static RegDescriptor div_and_mod(CGContext *cg_ctx, char ret_quotient,
                                 RegDescriptor reg_lhs, RegDescriptor reg_rhs) {

    Reg *reg_rax = cg_ctx->reg_pool.regs + REG_X86_64_RAX;
    Reg *reg_rdx = cg_ctx->reg_pool.regs + REG_X86_64_RDX;

    char is_rax_pushed = reg_rax->reg_in_use && ((reg_rhs != REG_X86_64_RAX) &&
                                                 (reg_lhs != REG_X86_64_RDX));
    char is_rdx_pushed = reg_rdx->reg_in_use && ((reg_rhs != REG_X86_64_RDX) &&
                                                 (reg_lhs != REG_X86_64_RDX));

    // Save RAX and RDX if they are in use.
    if (is_rax_pushed)
        file_emit_x86_64(cg_ctx, INST_X86_64_PUSH, OPERAND_TYPE_REG,
                         REG_X86_64_RAX);

    if (is_rdx_pushed)
        file_emit_x86_64(cg_ctx, INST_X86_64_PUSH, OPERAND_TYPE_REG,
                         REG_X86_64_RDX);

    // If RHS is RAX or RDX, we need to save it into a scratch register.
    RegDescriptor final_rhs = reg_rhs;
    char final_rhs_is_scratch = 0;
    if (reg_rhs == REG_X86_64_RDX || reg_rhs == REG_X86_64_RAX) {
        final_rhs = reg_alloc(cg_ctx);
        file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                         reg_rhs, final_rhs);
        final_rhs_is_scratch = 1;
    }

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG, reg_lhs,
                     REG_X86_64_RAX);

    file_emit_x86_64(cg_ctx, INST_X86_64_CQO);

    file_emit_x86_64(cg_ctx, INST_X86_64_IDIV, OPERAND_TYPE_REG, final_rhs);

    RegDescriptor res_reg;
    if (ret_quotient &&
        (reg_rhs == REG_X86_64_RAX || reg_lhs == REG_X86_64_RAX))
        res_reg = REG_X86_64_RAX;
    else if (!ret_quotient &&
             (reg_rhs == REG_X86_64_RDX || reg_lhs == REG_X86_64_RDX))
        res_reg = REG_X86_64_RDX;
    else {
        file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                         ret_quotient ? REG_X86_64_RAX : REG_X86_64_RDX,
                         reg_lhs);
        res_reg = reg_lhs;
    }

    if (final_rhs_is_scratch)
        reg_dealloc(cg_ctx, final_rhs);

    if (is_rax_pushed)
        file_emit_x86_64(cg_ctx, INST_X86_64_POP, OPERAND_TYPE_REG,
                         REG_X86_64_RAX);

    if (is_rdx_pushed)
        file_emit_x86_64(cg_ctx, INST_X86_64_POP, OPERAND_TYPE_REG,
                         REG_X86_64_RDX);

    if (reg_lhs != res_reg)
        reg_dealloc(cg_ctx, reg_lhs);

    if (reg_rhs != res_reg)
        reg_dealloc(cg_ctx, reg_rhs);

    return res_reg;
}

static RegDescriptor bit_shift(CGContext *cg_ctx,
                               Instructions_X86_64 shift_type,
                               RegDescriptor reg_lhs, RegDescriptor reg_rhs) {

    Reg *reg_rcx = cg_ctx->reg_pool.regs + REG_X86_64_RCX;
    char save_rcx = reg_rcx->reg_in_use &&
                    (reg_lhs != REG_X86_64_RCX && reg_rhs != REG_X86_64_RCX);

    // The amount by which the shifting needs to happen is stored in RHS, which
    // has to go into RCX.
    if (save_rcx)
        file_emit_x86_64(cg_ctx, INST_X86_64_XCHG, OPERAND_TYPE_REG_TO_REG,
                         REG_X86_64_RCX, reg_rhs);
    else if (reg_lhs == REG_X86_64_RCX)
        file_emit_x86_64(cg_ctx, INST_X86_64_XCHG, OPERAND_TYPE_REG_TO_REG,
                         reg_lhs, reg_rhs);
    else
        file_emit_x86_64(cg_ctx, INST_X86_64_XCHG, OPERAND_TYPE_REG_TO_REG,
                         reg_rhs, REG_X86_64_RCX);

    file_emit_x86_64(cg_ctx, shift_type, OPERAND_TYPE_REG, reg_lhs);

    if (save_rcx)
        file_emit_x86_64(cg_ctx, INST_X86_64_XCHG, OPERAND_TYPE_REG_TO_REG,
                         REG_X86_64_RCX, reg_rhs);

    reg_dealloc(cg_ctx, reg_rhs);

    return reg_lhs;
}

void code_gen_setup_func_call_arch_x86_64(CGContext *cg_ctx) {

    ArchData *arch_data = cg_ctx->arch_data;
    if (arch_data->func_call != FUNC_CALL_NONE)
        print_error(ERR_COMMON, "Function call can only be prepared for a "
                                "non NONE function call type");

    arch_data->is_rax_in_use = cg_ctx->reg_pool.regs[REG_X86_64_RAX].reg_in_use;

    if (arch_data->is_rax_in_use)
        file_emit_x86_64(cg_ctx, INST_X86_64_PUSH, OPERAND_TYPE_REG,
                         REG_X86_64_RAX);
}

void code_gen_ext_func_arg_arch_x86_64(CGContext *cg_ctx,
                                       RegDescriptor arg_reg) {

    ArchData *arch_data = cg_ctx->arch_data;
    switch (arch_data->func_call) {
    case FUNC_CALL_NONE:
        arch_data->func_call = FUNC_CALL_EXTERNAL;
        break;
    case FUNC_CALL_EXTERNAL:
        break;
    default:
        print_error(ERR_COMMON, "External function call arguments can only"
                                "be prepared for a FUNC_CALL_EXTERNAL type");
        break;
    }

    switch (cg_ctx->target_call_conv) {
    case TARGET_CALL_CONV_WIN:
        switch (arch_data->num_call_args++) {
        case 0:
            file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                             arg_reg, REG_X86_64_RCX);
            break;
        case 1:
            file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                             arg_reg, REG_X86_64_RDX);
            break;
        case 2:
            file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                             arg_reg, REG_X86_64_R8);
            break;
        case 3:
            file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                             arg_reg, REG_X86_64_R9);
            break;
        default:
            print_error(ERR_COMMON, "Unsupported amount of parameters for"
                                    "external function calls");
        }
        break;

    case TARGET_CALL_CONV_LINUX:
        print_error(ERR_DEV, "LINUX calling convention not implemented for"
                             "external function calls");
        break;

    default:
        print_error(ERR_DEV, "Encountered invalid calling convention for"
                             "external function calls");
    }
}

void code_gen_func_arg_arch_x86_64(CGContext *cg_ctx, RegDescriptor arg_reg) {

    ArchData *arch_data = cg_ctx->arch_data;
    switch (arch_data->func_call) {
    case FUNC_CALL_NONE:
        arch_data->func_call = FUNC_CALL_INTERNAL;
        break;
    case FUNC_CALL_INTERNAL:
        break;
    default:
        print_error(ERR_COMMON, "Internal function call arguments can only"
                                "be prepared for a FUNC_CALL_INTERNAL type");
        break;
    }

    arch_data->num_call_args += 1;
    file_emit_x86_64(cg_ctx, INST_X86_64_PUSH, OPERAND_TYPE_REG, arg_reg);
}

RegDescriptor code_gen_ext_func_call_arch_x86_64(CGContext *cg_ctx,
                                                 const char *func_name) {

    ArchData *arch_data = cg_ctx->arch_data;
    switch (arch_data->func_call) {
    case FUNC_CALL_NONE:
        arch_data->func_call = FUNC_CALL_EXTERNAL;
        break;
    case FUNC_CALL_EXTERNAL:
        break;
    default:
        print_error(ERR_COMMON, "External function call can only"
                                "be emitted for a FUNC_CALL_EXTERNAL type");
        break;
    }

    file_emit_x86_64(cg_ctx, INST_X86_64_CALL, OPERAND_TYPE_SYM, func_name);
    RegDescriptor ret_val_reg = copy_ret_val_from_rax(cg_ctx);
    return ret_val_reg;
}

RegDescriptor code_gen_func_call_arch_x86_64(CGContext *cg_ctx,
                                             RegDescriptor func_reg) {

    ArchData *arch_data = cg_ctx->arch_data;
    switch (arch_data->func_call) {
    case FUNC_CALL_NONE:
        arch_data->func_call = FUNC_CALL_INTERNAL;
        break;
    case FUNC_CALL_INTERNAL:
        break;
    default:
        print_error(ERR_COMMON, "Internal function call can only"
                                "be emitted for a FUNC_CALL_INTERNAL type");
        break;
    }

    file_emit_x86_64(cg_ctx, INST_X86_64_CALL, OPERAND_TYPE_REG, func_reg);
    RegDescriptor ret_val_reg = copy_ret_val_from_rax(cg_ctx);
    return ret_val_reg;
}

void code_gen_cleanup_arch_x86_64(CGContext *cg_ctx) {

    ArchData *arch_data = cg_ctx->arch_data;
    switch (arch_data->func_call) {
    case FUNC_CALL_EXTERNAL:
        file_emit_x86_64(cg_ctx, INST_X86_64_ADD, OPERAND_TYPE_IMM_TO_REG,
                         (int64_t)(arch_data->num_call_args * 8),
                         REG_X86_64_RSP);
        break;
    case FUNC_CALL_INTERNAL:
        break;
    default:
        print_error(ERR_COMMON, "No previous call to cleanup");
        break;
    }

    if (arch_data->is_rax_in_use)
        file_emit_x86_64(cg_ctx, INST_X86_64_POP, OPERAND_TYPE_REG,
                         REG_X86_64_RAX);
    else
        file_emit_x86_64(cg_ctx, INST_X86_64_ADD, OPERAND_TYPE_IMM_TO_REG,
                         (int64_t)8, REG_X86_64_RSP);

    arch_data->is_rax_in_use = 0;
    arch_data->num_call_args = 0;
    arch_data->func_call = FUNC_CALL_NONE;
}

void code_gen_get_global_addr_into_arch_x86_64(CGContext *cg_ctx,
                                               const char *sym,
                                               RegDescriptor target_reg) {
    file_emit_x86_64(cg_ctx, INST_X86_64_LEA, OPERAND_TYPE_SYM_TO_REG, sym,
                     REG_X86_64_RIP, target_reg);
}

void code_gen_get_local_addr_into_arch_x86_64(CGContext *cg_ctx, long offset,
                                              RegDescriptor target_reg) {
    file_emit_x86_64(cg_ctx, INST_X86_64_LEA, OPERAND_TYPE_MEM_TO_REG, offset,
                     REG_X86_64_RBP, target_reg);
}

void code_gen_get_global_into_arch_x86_64(CGContext *cg_ctx, const char *sym,
                                          RegDescriptor target_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_SYM_TO_REG, sym,
                     REG_X86_64_RIP, target_reg);
}

void code_gen_get_local_into_arch_x86_64(CGContext *cg_ctx, long offset,
                                         RegDescriptor target_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_MEM_TO_REG, offset,
                     REG_X86_64_RBP, target_reg);
}

void code_gen_store_global_arch_x86_64(CGContext *cg_ctx, const char *sym,
                                       RegDescriptor from_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_SYM, from_reg,
                     sym, REG_X86_64_RIP);
}

void code_gen_store_local_arch_x86_64(CGContext *cg_ctx, long offset,
                                      RegDescriptor from_reg) {
    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_MEM, from_reg,
                     offset, REG_X86_64_RBP);
}

void code_gen_store_arch_x86_64(CGContext *cg_ctx, RegDescriptor from_reg,
                                RegDescriptor mem_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_MEM, from_reg,
                     0, mem_reg);
}

void code_gen_add_imm_arch_x86_64(CGContext *cg_ctx, long data,
                                  RegDescriptor dest) {

    file_emit_x86_64(cg_ctx, INST_X86_64_ADD, OPERAND_TYPE_IMM_TO_REG,
                     (int64_t)data, dest);
}

void code_gen_branch_if_zero_arch_x86_64(CGContext *cg_ctx,
                                         RegDescriptor reg_desc,
                                         const char *jmp_label) {

    file_emit_x86_64(cg_ctx, INST_X86_64_TEST, OPERAND_TYPE_REG_TO_REG,
                     reg_desc);
    file_emit_x86_64(cg_ctx, INST_X86_64_JCC, JMP_TYPE_Z, jmp_label);
}

void code_gen_branch_arch_x86_64(CGContext *cg_ctx, const char *jmp_label) {

    file_emit_x86_64(cg_ctx, INST_X86_64_JMP, OPERAND_TYPE_SYM, jmp_label);
}

RegDescriptor code_gen_get_imm_arch_x86_64(CGContext *cg_ctx, long data) {
    RegDescriptor res_reg = reg_alloc(cg_ctx);
    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_IMM_TO_REG,
                     (int64_t)data, res_reg);
    return res_reg;
}

void code_gen_zero_out_reg_arch_x86_64(CGContext *cg_ctx,
                                       RegDescriptor reg_desc) {
    file_emit_x86_64(cg_ctx, INST_X86_64_XOR, OPERAND_TYPE_REG_TO_REG, reg_desc,
                     reg_desc);
}

void code_gen_copy_reg_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                   RegDescriptor dest_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG, src_reg,
                     dest_reg);
}

RegDescriptor code_gen_compare_arch_x86_64(CGContext *cg_ctx,
                                           ComparisonType comp_type,
                                           RegDescriptor lhs_reg,
                                           RegDescriptor rhs_reg) {

    if (comp_type > COMP_COUNT)
        print_error(ERR_COMMON, "Encountered invalid ComparisonType");

    RegDescriptor res_reg = reg_alloc(cg_ctx);
    file_emit_x86_64(cg_ctx, INST_X86_64_XOR, OPERAND_TYPE_REG_TO_REG, res_reg,
                     res_reg);

    file_emit_x86_64(cg_ctx, INST_X86_64_CMP, OPERAND_TYPE_REG_TO_REG, rhs_reg,
                     lhs_reg);
    file_emit_x86_64(cg_ctx, INST_X86_64_SETCC, comp_type, res_reg);

    reg_dealloc(cg_ctx, rhs_reg);
    reg_dealloc(cg_ctx, lhs_reg);

    return res_reg;
}

RegDescriptor code_gen_add_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg) {
    file_emit_x86_64(cg_ctx, INST_X86_64_ADD, OPERAND_TYPE_REG_TO_REG, src_reg,
                     dest_reg);
    reg_dealloc(cg_ctx, src_reg);
    return dest_reg;
}

RegDescriptor code_gen_sub_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_SUB, OPERAND_TYPE_REG_TO_REG, dest_reg,
                     src_reg);
    reg_dealloc(cg_ctx, dest_reg);
    return src_reg;
}

RegDescriptor code_gen_mul_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_IMUL, OPERAND_TYPE_REG_TO_REG, src_reg,
                     dest_reg);
    reg_dealloc(cg_ctx, src_reg);
    return dest_reg;
}

RegDescriptor code_gen_div_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg) {

    RegDescriptor res_reg = div_and_mod(cg_ctx, 1, src_reg, dest_reg);
    return res_reg;
}

RegDescriptor code_gen_mod_arch_x86_64(CGContext *cg_ctx, RegDescriptor src_reg,
                                       RegDescriptor dest_reg) {

    RegDescriptor res_reg = div_and_mod(cg_ctx, 0, src_reg, dest_reg);
    return res_reg;
}

RegDescriptor code_gen_shift_left_arch_x86_64(CGContext *cg_ctx,
                                              RegDescriptor src_reg,
                                              RegDescriptor dest_reg) {

    RegDescriptor res_reg =
        bit_shift(cg_ctx, INST_X86_64_SAL, src_reg, dest_reg);
    return res_reg;
}

RegDescriptor code_gen_shift_right_arithmetic_arch_x86_64(

    CGContext *cg_ctx, RegDescriptor src_reg, RegDescriptor dest_reg) {

    RegDescriptor res_reg =
        bit_shift(cg_ctx, INST_X86_64_SAR, src_reg, dest_reg);
    return res_reg;
}

void code_gen_allocate_on_stack_arch_x86_64(CGContext *cg_ctx, long size) {

    file_emit_x86_64(cg_ctx, INST_X86_64_SUB, OPERAND_TYPE_IMM_TO_REG,
                     (int64_t)size, REG_X86_64_RSP);
}

void code_gen_func_header_arch_x86_64(CGContext *cg_ctx) {

    file_emit_x86_64(cg_ctx, INST_X86_64_PUSH, OPERAND_TYPE_REG,
                     REG_X86_64_RBP);
    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                     REG_X86_64_RSP, REG_X86_64_RBP);
    file_emit_x86_64(cg_ctx, INST_X86_64_SUB, OPERAND_TYPE_IMM_TO_REG,
                     (int64_t)-cg_ctx->local_offset, REG_X86_64_RSP);
}

void code_gen_func_footer_arch_x86_64(CGContext *cg_ctx) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG,
                     REG_X86_64_RBP, REG_X86_64_RSP);
    file_emit_x86_64(cg_ctx, INST_X86_64_POP, OPERAND_TYPE_REG, REG_X86_64_RBP);
    file_emit_x86_64(cg_ctx, INST_X86_64_RET);
}

void code_gen_set_func_ret_val_arch_x86_64(CGContext *cg_ctx,
                                           RegDescriptor prev_reg) {

    file_emit_x86_64(cg_ctx, INST_X86_64_MOV, OPERAND_TYPE_REG_TO_REG, prev_reg,
                     REG_X86_64_RAX);
}

void code_gen_set_entry_point_arch_x86_64(CGContext *cg_ctx) {

    fprintf(cg_ctx->fptr_code,
            "%s"
            ".section .text\n"
            ".global main\n"
            "main:\n",
            cg_ctx->target_asm_dialect == TARGET_ASM_DIALECT_INTEL
                ? ".intel_syntax noprefix\n"
                : "");

    code_gen_func_header_arch_x86_64(cg_ctx);
}
