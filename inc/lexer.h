#ifndef __FILE_PROC_H__
#define __FILE_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../inc/helpers.h"

#define DELIMS " :=,;~()\r\n"
#define WHITESPACE " \r\n"

typedef struct lexed_tokens {
    char* token_start;
    int token_length;
    struct lexed_tokens *next_token;
} lexed_tokens;

typedef long int_cl;

long calculate_file_size(FILE *file_ptr);

void add_token_to_ll(lexed_tokens *new_token, lexed_tokens **root_token);

void print_lexed_tokens_ll(lexed_tokens *root_token);

void free_lexed_tokens_ll(lexed_tokens **root_token);

int strncmp_lexed_tokens(lexed_tokens *curr_token, char *str_to_cmp);

lexed_tokens* create_token(int token_length, char *data);

char* lex_token(char **file_data, lexed_tokens **curr_token);

void lex_file(char *file_dest);

#endif
