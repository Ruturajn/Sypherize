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
DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_32, 32)
DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_16, 16)
DEFINE_REG_NAME_LOOKUP_FUNC(get_reg_name_8, 8)

#undef REGISTER_NAME_64
#undef REGISTER_NAME_32
#undef REGISTER_NAME_16
#undef REGISTER_NAME_8

#undef DEFINE_REG_ENUM
#undef DEFINE_REG_NAME_LOOKUP_FUNC

CGContext *create_codegen_context_gnu_as_win(CGContext *parent_ctx) {
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
    new_ctx->target_fmt = TARGET_FMT_X86_64_GNU_AS;
    new_ctx->target_call_conv = TARGET_CALL_CONV_WIN;
    return new_ctx;
}

void free_codegen_context(CGContext *cg_ctx) {
    if (cg_ctx->parent_ctx == NULL) {
        free(cg_ctx->reg_pool.regs);
    }
    // Free environments.
    free(cg_ctx);
}

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
    INST_X86_64_CALL,
    INST_X86_64_CMP,
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
    OPERAND_TYPE_REG_TO_REG,
    OPERAND_TYPE_REG_TO_MEM,
} Instructions_Type_X86_64;

const char *inst_mnemonic_x86_64(Instructions_X86_64 inst,
                                 TargetFormat target) {
    switch (target) {
    default:
        break;
    }
    switch (inst) {
    default:
        print_error(
            ERR_COMMON,
            "Unable to convert Instructions_X86_64 to corresponding mnemonic");
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
    case INST_X86_64_CALL:
        return "call";
    case INST_X86_64_CMP:
        return "cmp";
    }
    return "";
}

void file_emit_x86_64_imm_to_mem(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // immediate value, memory offset, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    int64_t mem_offset = va_arg(operands, int64_t);
    RegDescriptor mem_reg = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s $%" PRId64 ", %" PRId64 "(%s)\n", mnemonic, imm_val,
            mem_offset, get_reg_name(mem_reg));
}

void file_emit_x86_64_imm_to_reg(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // immediate value, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s $%" PRId64 ", %s\n", mnemonic, imm_val,
            get_reg_name(dest_reg));
}

void file_emit_x86_64_mem_to_reg(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // memory offset, source, destination.
    int64_t imm_val = va_arg(operands, int64_t);
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s %" PRId64 "(%s), %s\n", mnemonic, imm_val,
            get_reg_name(src_reg), get_reg_name(dest_reg));
}

void file_emit_x86_64_sym_to_reg(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // name, source, destination.
    const char *sym = va_arg(operands, const char *);
    RegDescriptor src_reg = va_arg(operands, RegDescriptor);
    RegDescriptor dest_reg = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s %s(%s), %s\n", mnemonic, sym, get_reg_name(src_reg),
            get_reg_name(dest_reg));
}

void file_emit_x86_64_reg_to_reg(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // source, destination.
    RegDescriptor reg_src = va_arg(operands, RegDescriptor);
    RegDescriptor reg_dest = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s %s, %s\n", mnemonic, get_reg_name(reg_src),
            get_reg_name(reg_dest));
}

void file_emit_x86_64_reg_to_mem(FILE *fptr_code, CGContext *cg_ctx,
                                 const char *mnemonic, va_list operands) {

    // source, memory offset, destination.
    RegDescriptor reg_src = va_arg(operands, RegDescriptor);
    int64_t mem_offset = va_arg(operands, int64_t);
    RegDescriptor reg_dest = va_arg(operands, RegDescriptor);
    fprintf(fptr_code, "%s %s, %" PRId64 "(%s)\n", mnemonic,
            get_reg_name(reg_src), mem_offset, get_reg_name(reg_dest));
}

void file_emit_x86_64_inst(FILE *fptr_code, CGContext *cg_ctx,
                           Instructions_X86_64 inst, ...) {
    va_list operands;
    va_start(operands, inst);

    if (cg_ctx == NULL)
        print_error(ERR_COMMON, "Encountered NULL code gen context");

    const char *mnemonic = inst_mnemonic_x86_64(inst, cg_ctx->target_fmt);

    switch (cg_ctx->target_fmt) {

    default:
        print_error(ERR_COMMON,
                    "Unhandled target format in file_emit_x86_64_inst()");
        break;

    case TARGET_FMT_X86_64_GNU_AS:
        switch (inst) {

        default:
            print_error(ERR_COMMON,
                        "Unhandled instruction in file_emit_x86_64_inst()");
            break;

        case INST_X86_64_ADD:
        case INST_X86_64_SUB:
        case INST_X86_64_MOV:;
            Instructions_Type_X86_64 inst_type =
                va_arg(operands, Instructions_Type_X86_64);
            switch (inst_type) {

            default:
                print_error(
                    ERR_DEV,
                    "Unhandled instruction type in file_emit_x86_64_inst()");
                break;

            case OPERAND_TYPE_IMM_TO_MEM:;
                file_emit_x86_64_imm_to_mem(fptr_code, cg_ctx, mnemonic,
                                            operands);
                break;

            case OPERAND_TYPE_IMM_TO_REG:;
                file_emit_x86_64_imm_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);
                break;

            case OPERAND_TYPE_MEM_TO_REG:;
                file_emit_x86_64_mem_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);
                break;

            case OPERAND_TYPE_REG_TO_REG:;
                file_emit_x86_64_reg_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);
                break;

            case OPERAND_TYPE_REG_TO_MEM:
                file_emit_x86_64_reg_to_mem(fptr_code, cg_ctx, mnemonic,
                                            operands);
                break;
            }
            break;

        case INST_X86_64_LEA:
            inst_type = va_arg(operands, Instructions_Type_X86_64);
            if (inst_type == OPERAND_TYPE_SYM_TO_REG) {
                file_emit_x86_64_sym_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);

            } else if (inst_type == OPERAND_TYPE_MEM_TO_REG) {
                file_emit_x86_64_mem_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);

            } else
                print_error(ERR_DEV,
                            "Invalid operand type for `lea` instruction");
            break;

        case INST_X86_64_MUL:
            break;

        case INST_X86_64_IMUL:
            inst_type = va_arg(operands, Instructions_Type_X86_64);
            if (inst_type == OPERAND_TYPE_MEM_TO_REG) {
                file_emit_x86_64_mem_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);

            } else if (inst_type == OPERAND_TYPE_REG_TO_REG) {
                file_emit_x86_64_reg_to_reg(fptr_code, cg_ctx, mnemonic,
                                            operands);

            } else
                print_error(ERR_DEV,
                            "Invalid operand type for `imul` instruction");

            break;

        case INST_X86_64_DIV:
            break;

        case INST_X86_64_IDIV:
            inst_type = va_arg(operands, Instructions_Type_X86_64);
            if (inst_type == OPERAND_TYPE_MEM) {
                int64_t mem_offset = va_arg(operands, int64_t);
                RegDescriptor reg_desc = va_arg(operands, RegDescriptor);
                fprintf(fptr_code, "%s %" PRId64 "(%s)\n", mnemonic, mem_offset,
                        get_reg_name(reg_desc));

            } else if (inst_type == OPERAND_TYPE_REG) {
                RegDescriptor reg_desc = va_arg(operands, RegDescriptor);
                fprintf(fptr_code, "%s %s\n", mnemonic, get_reg_name(reg_desc));

            } else
                print_error(ERR_DEV,
                            "Invalid operand type for `idiv` instruction");
            break;

        case INST_X86_64_PUSH:;
            inst_type = va_arg(operands, Instructions_Type_X86_64);
            if (inst_type == OPERAND_TYPE_REG) {
                RegDescriptor push_reg = va_arg(operands, RegDescriptor);
                fprintf(fptr_code, "%s %s\n", mnemonic, get_reg_name(push_reg));

            } else if (inst_type == OPERAND_TYPE_MEM) {
                int64_t mem_offset = va_arg(operands, int64_t);
                RegDescriptor push_reg = va_arg(operands, RegDescriptor);
                fprintf(fptr_code, "%s %" PRId64 "(%s)\n", mnemonic, mem_offset,
                        get_reg_name(push_reg));

            } else if (inst_type == OPERAND_TYPE_IMM) {
                int64_t imm_val = va_arg(operands, int64_t);
                fprintf(fptr_code, "%s $%" PRId64 "\n", mnemonic, imm_val);

            } else
                print_error(ERR_DEV,
                            "Invalid operand type for `push` instruction");
            break;

        case INST_X86_64_POP:
            break;

        case INST_X86_64_RET:
            break;

        case INST_X86_64_XOR:
            break;

        case INST_X86_64_CALL:
            inst_type = va_arg(operands, Instructions_Type_X86_64);
            if (inst_type == OPERAND_TYPE_SYM) {
                const char *sym = va_arg(operands, const char *);
                fprintf(fptr_code, "%s %s\n", mnemonic, sym);

            } else if (inst_type == OPERAND_TYPE_REG) {
                RegDescriptor call_reg = va_arg(operands, RegDescriptor);
                fprintf(fptr_code, "%s *%s\n", mnemonic,
                        get_reg_name(call_reg));

            } else
                print_error(ERR_DEV,
                            "Invalid operand type for `call` instruction");
            break;

        case INST_X86_64_CMP:
            break;
        }
        break;
    }
    va_end(operands);
}
