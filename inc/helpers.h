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
            print_error(msg, 1);                                               \
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

/**
 * @brief This functions takes in the error message, and prints it out, in
 *        a specific format.
 * @param msg     [char *] Message to be printed.
 * @param is_exit [int] `1` for exit, `0` to just print the message and
 *                continue.
 */
void print_error(char *msg, int is_exit);

#endif /* __HELPERS_H__ */
