#ifndef __HELPERS_H__
#define __HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_NULL(ptr, msg) {\
    if (ptr == NULL) { \
        print_error(msg); exit(EXIT_FAILURE); } }

#define MEM_ERR       "Could not allocate memory"
#define FILE_OPEN_ERR "Could not access file"

void print_error(char *msg);

#endif
