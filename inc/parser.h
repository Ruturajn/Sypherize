#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void print_ast_nodes_type(ast_nodes *curr_node);

void print_ast_nodes(ast_nodes *root_node, int indent);

int parse_int(lexed_tokens *token, ast_nodes *node);

void parse_tokens(lexed_tokens *curr_token, ast_nodes curr_node);

#endif
