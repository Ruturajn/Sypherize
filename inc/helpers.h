#ifndef __HELPERS_H__
#define __HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_NULL(ptr, msg) { if (ptr == NULL) { print_error(msg, 1); } }

#define MEM_ERR       "Could not allocate memory"
#define FILE_OPEN_ERR "Could not access file"
#define FILE_SIZE_ERR "Could not calculate file size"

void print_error(char *msg, int is_exit);

#endif /* __HELPERS_H__ */
