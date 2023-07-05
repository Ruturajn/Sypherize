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

#ifdef __cplusplus
}
#endif

#endif /* __HELPERS_H__ */
