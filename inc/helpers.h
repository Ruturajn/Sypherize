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
#define CHECK_NULL(ptr, msg)                                                   \
    {                                                                          \
        if (ptr == NULL) {                                                     \
            print_error(msg, 1, NULL);                                         \
        }                                                                      \
    }

/**
 * @brief Checks if a `ptr` is at the end of file, is so calls `print_error`
 *        with exit enabled.
 */
#define CHECK_END(ptr, msg, token)                                             \
    {                                                                          \
        if (ptr == '\0') {                                                     \
            print_error(msg, 1, token);                                        \
        }                                                                      \
    }

#define MEM_ERR                                                                \
    "Could not allocate memory" ///< String for memory
                                ///< allocation error.
#define FILE_OPEN_ERR                                                          \
    "Could not access file" ///< String for file access errors.
#define FILE_SIZE_ERR                                                          \
    "Could not calculate file size" ///< String for file size
                                    ///< errors.
#define SYNTAX_ERR "Invalid Syntax" ///< String for Syntax error.
#define VAR_REDEFINITION_ERR                                                   \
    "Redfinition of Variable" ///< String for redfinition
                              ///< of a variable error.

#endif /* __HELPERS_H__ */
