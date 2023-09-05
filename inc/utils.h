#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

#define USAGE_STRING                                                           \
    "\033[1;36mSypherc 0.1\033[1;37m\n"                                        \
    "Ruturajn <nanotiruturaj@gmail.com>\n"                                     \
    "A Compiler for Sypher\n"                                                  \
    "\n"                                                                       \
    "\033[1;33mUSAGE:\033[1;37m\n"                                             \
    "    sypherc [OPTIONS] INPUT_FILE\n"                                       \
    "\n"                                                                       \
    "\033[1;33mOPTIONS:\033[1;37m\n"                                           \
    "    \033[1;35m-cc, --call-conv <CALLING_CONVENTION>\033[1;37m\n"          \
    "            A valid calling convention for code generation\n"             \
    "            VALID CALLING CONVENTIONS:\n"                                 \
    "            - `default`\n"                                                \
    "            - `linux`\n"                                                  \
    "            - `windows`\n"                                                \
    "\n"                                                                       \
    "\033[1;33mOPTIONS:\033[1;37m\n"                                           \
    "    \033[1;35m-ad, --asm-dialect <ASSEMBLY_DIALECT>\033[1;37m\n"          \
    "            A valid assembly dialect for code generation\n"               \
    "            VALID ASSEMBLY DIALECTS:\n"                                   \
    "            - `default`\n"                                                \
    "            - `att`\n"                                                    \
    "            - `intel`\n"                                                  \
    "\n"                                                                       \
    "    \033[1;35m-f, --format <OUTPUT_FORMAT>\033[1;37m\n"                   \
    "            A valid output format for code generation\n"                  \
    "            VALID FORMATS:\n"                                             \
    "            - `default`\n"                                                \
    "            - `x86_64-gnu-as`\n"                                          \
    "\n"                                                                       \
    "    \033[1;35m-h, --help\033[1;37m\n"                                     \
    "            Print this help information\n"                                \
    "\n"                                                                       \
    "    \033[1;35m-i, --input <INPUT_FILE_PATH>\033[1;37m\n"                  \
    "            Path to the input file\n"                                     \
    "\n"                                                                       \
    "    \033[1;35m-o, --output <OUTPUT_FILE_PATH>\033[1;37m\n"                \
    "            Path to the output file\n"                                    \
    "\n"                                                                       \
    "    \033[1;35m-v, --version\033[1;37m\n"                                  \
    "            Print out current version of Sypherize\n"                     \
    "\n"                                                                       \
    "    \033[1;35m-V, --verbose\033[1;37m\n"                                  \
    "            Print out extra debugging information\n"                      \
    "\n"                                                                       \
    "NOTE - Everything else is treated as an input file\n"                     \
    "\n"

#define VERSION_STRING "Sypherize 0.1\n"

/**
 * @brief Checks if a `ptr` is NULL, is so calls `print_error`, with exit
 *        enabled.
 */
#define CHECK_NULL(ptr, fmt, str)                                              \
    {                                                                          \
        if (ptr == NULL) {                                                     \
            print_error(ERR_MEM, fmt, str);                                    \
        }                                                                      \
    }

/**
 * @brief Checks if a `ptr` is at the end of file, is so calls `print_error`
 *        with exit enabled.
 */
#define CHECK_END(ptr, fmt, str)                                               \
    {                                                                          \
        if (ptr == '\0') {                                                     \
            print_error(ERR_EOF, fmt, str);                                    \
        }                                                                      \
    }

typedef enum ErrType {
    ERR_COMMON = 0,
    ERR_ARGS,
    ERR_MEM,
    ERR_FILE_OPEN,
    ERR_FILE_SIZE,
    ERR_FILE_READ,
    ERR_SYNTAX,
    ERR_REDEFINITION,
    ERR_EOF,
    ERR_TYPE,
    ERR_DEV,

    ERR_COUNT,
} ErrType;

extern const char *err_strings[ERR_COUNT];

void print_error(ErrType err, const char *fmt, ...);

void print_warning(ErrType err, const char *fmt, ...);

char *read_file_data(char *file_dest);

/**
 * @brief  Calculates the size of the file, pointed by `file_ptr`.
 *
 * @param  file_ptr  [`FILE *`] pointer to the file, for which the
 *                   size needs to be calculated.
 * @return size_t    The size of the file.
 */
size_t calculate_file_size(FILE *file_ptr);

#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H__ */
