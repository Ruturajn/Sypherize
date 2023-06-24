#ifndef __FILE_PROC_H__
#define __FILE_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../inc/helpers.h"

#define DELIMS " :=,;~()\r\n"
#define WHITESPACE " \r\n"

typedef struct lexed_token {
    char* token_start;
    int token_length;
} lexed_token;

typedef long int_cl;

long calculate_file_size(FILE *file_ptr);

void print_lexed_token(lexed_token *curr_token);

int strncmp_lexed_token(lexed_token *curr_token, char *str_to_cmp);

lexed_token* create_token(int token_length, char *data);

char* lex_token(char **file_data, lexed_token **curr_token);

void lex_file(char *file_dest);

#endif /* __FILE_PROC_H__ */
