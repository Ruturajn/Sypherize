#ifndef __FILE_PROC_H__
#define __FILE_PROC_H__

#include "../inc/helpers.h"

#define DELIMS " :=,;~()\r\n"
#define WHITESPACE " \r\n"

typedef struct lexed_tokens {
    char* token_start;
    int token_length;
    struct lexed_tokens *next_token;
} lexed_tokens;

typedef long int_cl;

typedef struct ast_nodes {
    enum node_type {
        TYPE_NULL = 0,
        TYPE_INT,
        TYPE_ROOT,
    } type;
    int_cl node_val;
    struct ast_nodes *child;
    struct ast_nodes *next_child;
} ast_nodes;

long calculate_file_size(FILE *file_ptr);

void lex_file(char *file_dest);

void add_token_to_ll(lexed_tokens *new_token, lexed_tokens **root_token);

void print_lexed_tokens_ll(lexed_tokens *root_node);

void free_lexed_tokens_ll(lexed_tokens **root_node);

#endif
