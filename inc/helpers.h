#ifndef __HELPERS_H__
#define __HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if a `ptr` is NULL, is so calls `print_error`, with exit
 *        enabled.
 */
#define CHECK_NULL(ptr, fmt, str)                                              \
    {                                                                          \
        if (ptr == NULL) {                                                     \
            print_error(ERR_MEM, fmt, str, 0);                                 \
        }                                                                      \
    }

/**
 * @brief Checks if a `ptr` is at the end of file, is so calls `print_error`
 *        with exit enabled.
 */
#define CHECK_END(ptr, fmt, str)                                               \
    {                                                                          \
        if (ptr == '\0') {                                                     \
            print_error(ERR_EOF, fmt, str, 0);                                 \
        }                                                                      \
    }

typedef enum ErrType {
    ERR_COMMON = 0,
    ERR_NUM_ARGS,
    ERR_MEM,
    ERR_FILE_OPEN,
    ERR_FILE_SIZE,
    ERR_FILE_READ,
    ERR_SYNTAX,
    ERR_REDEFINITION,
    ERR_EOF,
} ErrType;

extern const char *err_strings[];

/**
 * @brief This functions takes in the error message, and prints it out, in
 *        a specific format.
 *        TODO: Add `perror` type error handling.
 *
 * @param msg      [char *] Message to be printed.
 * @param is_exit  [int] `1` for exit, `0` to just print the message and
 *                 continue.
 * @param token    [`LexedToken *`] Pointer to the token if it needs to be
 *                 printed out.
 */
void print_error(ErrType err, char *fmt, char *str, int val);

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

#endif /* __HELPERS_H__ */
