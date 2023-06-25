#include "../inc/helpers.h"

void print_error(char *msg, int is_exit) {
    printf("\033[1;31m[ERROR] : %s!\n\033[1;37m", msg);
    if (is_exit)
        exit(1);
}
