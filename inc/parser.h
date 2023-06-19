#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ast_node {
    enum node_type {
        TYPE_NULL = 0,
        TYPE_INT,
        TYPE_ROOT,
    } type;
    int_cl node_val;
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

int parse_int(lexed_tokens *token, ast_node *node);

void parse_tokens(lexed_tokens *curr_token, ast_node curr_node);

#endif
