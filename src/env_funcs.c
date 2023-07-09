#include "../inc/env_funcs.h"
#include "../inc/ast_funcs.h"
#include "../inc/helpers.h"
#include "../inc/parser.h"

Env *create_env(Env *parent_env) {
    Env *new_env = calloc(1, sizeof(Env));
    CHECK_NULL(new_env, "Unable to allocate memory for new environment", NULL);
    new_env->parent_env = parent_env;
    new_env->binding = NULL;
    return new_env;
}

int set_env(Env **env_to_set, AstNode *identifier_node, AstNode *id_val) {
    if (env_to_set == NULL || identifier_node == NULL || id_val == NULL)
        return 0;

    IdentifierBind *temp = (*env_to_set)->binding;

    IdentifierBind *binds = calloc(1, sizeof(IdentifierBind));
    CHECK_NULL(binds, "Unable to allocate memory for new binding", NULL);
    binds->identifier = identifier_node;
    binds->id_val = id_val;

    if (temp == NULL) {
        (*env_to_set)->binding = binds;
        return 1;
    }

    while (temp != NULL) {
        if (node_cmp(temp->identifier, identifier_node)) {
            temp->id_val = id_val;
            return 1;
        }
        temp = temp->next_id_bind;
    }

    binds->next_id_bind = (*env_to_set)->binding;
    (*env_to_set)->binding = binds;
    return 1;
}

AstNode *get_env(Env *env_to_get, AstNode *identifier, int *stat) {
    IdentifierBind *curr_bind = env_to_get->binding;
    AstNode *val = node_alloc();
    while (curr_bind != NULL) {
        if (node_cmp(curr_bind->identifier, identifier)) {
            *stat = 1;
            copy_node(val, curr_bind->id_val);
            return val;
        }
        curr_bind = curr_bind->next_id_bind;
    }
    *stat = 0;
    return val;
}

AstNode *parser_get_type(ParsingContext *context, AstNode *identifier,
                         int *stat) {
    ParsingContext *temp_ctx = context;
    int status = -1;
    while (temp_ctx != NULL) {
        AstNode *res = get_env(temp_ctx->env_type, identifier, &status);
        if (status) {
            *stat = 1;
            return res;
        }
        temp_ctx = temp_ctx->parent_ctx;
    }
    AstNode *res = create_node_none();
    *stat = 0;
    return res;
}

AstNode *parser_get_func(ParsingContext *context, AstNode *identifier,
                         int *stat) {
    ParsingContext *temp_ctx = context;
    int status = -1;
    while (temp_ctx != NULL) {
        AstNode *res = get_env(temp_ctx->funcs, identifier, &status);
        if (status) {
            *stat = 1;
            return res;
        }
        temp_ctx = temp_ctx->parent_ctx;
    }
    AstNode *res = create_node_none();
    *stat = 0;
    return res;
}