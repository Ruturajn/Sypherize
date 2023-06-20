#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ast_node {
    enum node_type {
        TYPE_NULL = 0,
        TYPE_ROOT,
        TYPE_INT,
        TYPE_BINARY_OPERATOR,
        TYPE_VAR_DECLARATION,
        TYPE_VAR_INIT,
        TYPE_SYM,
    } type;
    union node_val {
        long val;
        char *node_symbol;
    } ast_val;
    struct ast_node *child;
    struct ast_node *next_child;
} ast_node;

typedef struct identifier_bind {
    ast_node identifier;
    ast_node id_val;
    struct identifier_bind *next_id_bind;
} identifier_bind;

typedef struct env {
    identifier_bind *binding;
    struct env *parent_env;
} env;

void print_ast_node(ast_node *root_node, int indent);

int parse_int(lexed_token *token, ast_node *node);

char* parse_tokens(char **temp_file_data, lexed_token *curr_token,
                  ast_node *curr_node);

#endif
