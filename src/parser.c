#include "../inc/parser.h"

void print_ast_node(ast_node *root_node, int indent) {
    if (root_node == NULL) { return; }
    for (int i=0; i < indent; i++)
        putchar(' ');
    switch(root_node->type) {
        case TYPE_ROOT:
            printf("ROOT\n");
            break;
        case TYPE_INT:
            printf("INT : %ld\n", root_node->node_val);
            break;
        case TYPE_NULL:
            printf("NULL\n");
            break;
        default:
            printf("Unknown TYPE\n");
            break;
    }
    putchar('\n');
    ast_node *child_node = root_node->child;
    while (child_node != NULL) {
        print_ast_node(child_node, indent + 4);
        child_node = child_node->next_child;
    }
}

int parse_int(lexed_tokens *token, ast_node *node) {
    if (token == NULL || node == NULL) { return 2; }
    if (token->token_length == 0 && *(token->token_start) == '0') {
        node->type = TYPE_INT;
        node->node_val = 0;
    } else {
        long temp = strtol(token->token_start, NULL, 10);
        if (temp != 0) {
            node->type = TYPE_INT;
            node->node_val = temp;
        }
        else { return 0; }
    }
    return 1;
}

env* create_env(env *parent_env) {
    env *new_env = calloc(1, sizeof(env));
    CHECK_NULL(new_env, "Could not allocate memory");
    new_env->parent_env = parent_env;
    new_env->binding = NULL;
    return new_env;
}

void set_env(env env_to_set, ast_node identifier_node, ast_node id_val) {
    identifier_bind *binds = calloc(1, sizeof(identifier_bind));
    CHECK_NULL(binds, "Could not allocate memory");
    binds->identifier = identifier_node;
    binds->id_val = id_val;
    binds->next_id_bind = env_to_set.binding;
    env_to_set.binding = binds;
}

int node_cmp(ast_node *node1, ast_node *node2) {
    if (node1 == NULL && node2 == NULL)
        return 1;
    if ((node1 == NULL && node2 != NULL) ||
         (node1 != NULL && node2 == NULL ))
        return 0;

    if (node1->type != node2->type) { return 0; }

    switch(node1->type) {
        case TYPE_NULL:
            if (node2->type == TYPE_NULL)
                return 1;
            break;
        case TYPE_INT:
            if (node1->node_val == node2->node_val)
                return 1;
            else
                return 0;
            break;
        case TYPE_ROOT:
            printf("Compare Programs : Not implemented\n");
            break;
        default:
            break;
    }

    return 0;
}

ast_node get_env(env env_to_get, ast_node identifier) {
    identifier_bind *curr_bind = env_to_get.binding;
    while (curr_bind != NULL) {
        if (node_cmp(&(curr_bind->identifier), &identifier))
            return curr_bind->id_val;
        curr_bind = curr_bind->next_id_bind;
    }
    ast_node val;
    val.node_val = 0;
    val.type = TYPE_NULL;
    val.child = NULL;
    val.next_child = NULL;
    return val;
}

void parse_tokens(lexed_tokens *curr_token, ast_node curr_node) {

    if (parse_int(curr_token, &curr_node)) {
        // If this is an integer, look for a valid operator.
        printf("Found integer\n");
    } else {
        // If the token was not an integer, check if it is
        // variable declaration, assignment, etc.
        printf("Not an Integer\n");
    }

    // add_token_to_ll(curr_token, &root_token);
    print_ast_node(&curr_node, 0);
}
