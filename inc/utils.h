#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#define FUNC_NORETURN __attribute__((noreturn))
#define FUNC_FORMAT(...) __attribute__((format(__VA_ARGS__)))
#else
#define FUNC_NORETURN
#define FUNC_FORMAT(...)
#endif

#define USAGE_STRING                                                                               \
    "Sypherc 0.1\n"                                                                                \
    "Ruturajn <nanotiruturaj@gmail.com>\n"                                                         \
    "Compiler for Sypher\n"                                                                        \
    "\n"                                                                                           \
    "USAGE:\n"                                                                                     \
    "    sypherc [OPTIONS] INPUT_FILE\n"                                                           \
    "\n"                                                                                           \
    "OPTIONS:\n"                                                                                   \
    "    -f, --format <OUTPUT_FORMAT>\n"                                                           \
    "            A valid output format for code generation\n"                                      \
    "            VALID FORMATS:\n"                                                                 \
    "            - `x86_64-windows`\n"                                                             \
    "            - `default`\n"                                                                    \
    "\n"                                                                                           \
    "    -h, --help\n"                                                                             \
    "            Print this help information\n"                                                    \
    "\n"                                                                                           \
    "    -i, --input <INPUT_FILE_PATH>\n"                                                          \
    "            Path to the input file\n"                                                         \
    "\n"                                                                                           \
    "    -o, --output <OUTPUT_FILE_PATH>\n"                                                        \
    "            Path to the output file\n"                                                        \
    "\n"                                                                                           \
    "    -v, --version\n"                                                                          \
    "            Print out current version of Sypherize\n"                                         \
    "\n"                                                                                           \
    "    -V, --verbose\n"                                                                          \
    "            Print out extra debugging information\n"                                          \
    "\n"                                                                                           \
    "NOTE - Everything else is treated as an input file\n"                                         \
    "\n"

#define VERSION_STRING "Sypherize 0.1\n"

/**
 * @brief Checks if a `ptr` is NULL, is so calls `print_error`, with exit
 *        enabled.
 */
#define CHECK_NULL(ptr, fmt, str)                                                                  \
    {                                                                                              \
        if (ptr == NULL) {                                                                         \
            print_error(ERR_MEM, fmt, str, 0);                                                     \
        }                                                                                          \
    }

/**
 * @brief Checks if a `ptr` is at the end of file, is so calls `print_error`
 *        with exit enabled.
 */
#define CHECK_END(ptr, fmt, str)                                                                   \
    {                                                                                              \
        if (ptr == '\0') {                                                                         \
            print_error(ERR_EOF, fmt, str, 0);                                                     \
        }                                                                                          \
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
} ErrType;

extern const char *err_strings[];

void print_error(ErrType err, char *fmt, char *str, int val);

void print_warning(ErrType err, char *fmt, char *str, int val);

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
