#ifndef __FILE_PROC_H__
#define __FILE_PROC_H__

#include "../inc/helpers.h"

typedef struct lexed_tokens {
    char* token_start;
    int token_length;
    struct lexed_tokens *next_token;
} lexed_tokens;

long calculate_file_size(FILE *file_ptr);

void lex_file(char *file_dest);

void add_token_to_ll(int token_length, lexed_tokens **root_node, char *data);

void print_lexed_tokens_ll(lexed_tokens *root_node);

void free_lexed_tokens_ll(lexed_tokens **root_node);

#endif
