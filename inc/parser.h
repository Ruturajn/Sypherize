#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AstNode {
    enum NodeType {
        TYPE_NULL = 0,
        TYPE_PROGRAM,
        TYPE_ROOT,
        TYPE_INT,
        TYPE_BINARY_OPERATOR,
        TYPE_VAR_DECLARATION,
        TYPE_VAR_INIT,
        TYPE_SYM,
    } type;
    union NodeVal {
        long val;
        char *node_symbol;
    } ast_val;
    struct AstNode *child;
    struct AstNode *next_child;
} AstNode;

typedef struct IdentifierBind {
    AstNode *identifier;
    AstNode *id_val;
    struct IdentifierBind *next_id_bind;
} IdentifierBind;

typedef struct Env {
    IdentifierBind *binding;
    struct Env *parent_env;
} Env;

typedef struct ParsingContext {
    Env *env_type;
    Env *vars;
} ParsingContext;

void print_ast_node(AstNode *root_node, int indent);

int parse_int(LexedToken *token, AstNode *node);

Env* create_env(Env *parent_Env);

int set_env(Env *Env_to_set, AstNode *identifier_node, AstNode *id_val);

int node_cmp(AstNode *node1, AstNode *node2);

AstNode *get_env(Env *Env_to_get, AstNode *identifier, int *stat);

void add_ast_node_child(AstNode *parent_node, AstNode *child_to_add);

ParsingContext *create_parsing_context();

AstNode *node_alloc();

AstNode *node_symbol_create(char *symbol_str);

AstNode *node_int_create(long val);

void free_node(AstNode *node_to_free);

AstNode *node_symbol_from_token_create(LexedToken *token);

char* parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode *curr_node, ParsingContext *context);

#endif /* __PARSER_H__ */
