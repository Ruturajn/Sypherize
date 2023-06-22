#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ast_node {
    enum node_type {
        TYPE_NULL = 0,
        TYPE_PROGRAM,
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
    ast_node *identifier;
    ast_node *id_val;
    struct identifier_bind *next_id_bind;
} identifier_bind;

typedef struct env {
    identifier_bind *binding;
    struct env *parent_env;
} env;

typedef struct parsing_context {
    env *env_type;
    env *vars;
} parsing_context;

void print_ast_node(ast_node *root_node, int indent);

int parse_int(lexed_token *token, ast_node *node);

char* parse_tokens(char **temp_file_data, lexed_token *curr_token,
                  ast_node *curr_node);

env* create_env(env *parent_env);

int set_env(env *env_to_set, ast_node *identifier_node, ast_node *id_val);

int node_cmp(ast_node *node1, ast_node *node2);

ast_node *get_env(env *env_to_get, ast_node *identifier);

void add_ast_node_child(ast_node *parent_node, ast_node *child_to_add);

parsing_context *create_parsing_context();

ast_node *node_alloc();

ast_node *node_symbol_create(char *symbol_str);

ast_node *node_int_create(long val);

ast_node *node_symbol_from_token_create(enum node_type type, lexed_token *token);

char* parse_tokens(char **temp_file_data, lexed_token *curr_token, 
                   ast_node *curr_node);

#endif
