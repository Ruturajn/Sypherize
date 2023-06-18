#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CHECK_NULL(ptr, msg) {\
    if (ptr == NULL) { \
        print_error(msg); exit(EXIT_FAILURE); } }

void print_error(char *msg);

#endif
