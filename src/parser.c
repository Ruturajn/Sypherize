#include "../inc/parser.h"

void print_ast_node(ast_node *root_node, int indent) {
    if (root_node == NULL) { return; }
    for (int i=0; i < indent; i++)
        putchar(' ');
    switch(root_node->type) {
        case TYPE_NULL:
            printf("NULL");
            break;
        case TYPE_ROOT:
            printf("ROOT");
            break;
        case TYPE_INT:
            printf("INT : %ld", root_node->ast_val.val);
            break;
        case TYPE_BINARY_OPERATOR:
            printf("BINARY OP : %s", root_node->ast_val.node_symbol);
            break;
        case TYPE_VAR_DECLARATION:
            printf("VAR DECL");
            break;
        case TYPE_VAR_INIT:
            printf("VAR INIT");
            break;
        case TYPE_SYM:
            printf("SYM : %s", root_node->ast_val.node_symbol);
            break;
        default:
            printf("Unknown TYPE");
            break;
    }
    putchar('\n');
    ast_node *child_node = root_node->child;
    while (child_node != NULL) {
        print_ast_node(child_node, indent + 4);
        child_node = child_node->next_child;
    }
}

int parse_int(lexed_token *token, ast_node *node) {
    if (token == NULL || node == NULL) { return 2; }
    if (token->token_length == 0 && *(token->token_start) == '0') {
        node->type = TYPE_INT;
        node->ast_val.val = 0;
    } else {
        long temp = strtol(token->token_start, NULL, 10);
        if (temp != 0) {
            node->type = TYPE_INT;
            node->ast_val.val = temp;
        }
        else { return 0; }
    }
    return 1;
}

env* create_env(env *parent_env) {
    env *new_env = calloc(1, sizeof(env));
    CHECK_NULL(new_env, MEM_ERR);
    new_env->parent_env = parent_env;
    new_env->binding = NULL;
    return new_env;
}

void set_env(env env_to_set, ast_node identifier_node, ast_node id_val) {
    identifier_bind *binds = calloc(1, sizeof(identifier_bind));
    CHECK_NULL(binds, MEM_ERR);
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
            if (node1->ast_val.val == node2->ast_val.val)
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
    val.ast_val.val = 0;
    val.type = TYPE_NULL;
    val.child = NULL;
    val.next_child = NULL;
    return val;
}

void add_ast_node_child(ast_node *parent_node, ast_node *child_to_add) {
    if (parent_node == NULL || child_to_add == NULL) { return; }

    ast_node *new_child = (ast_node *)calloc(1, sizeof(ast_node));
    CHECK_NULL(new_child, MEM_ERR);
    *new_child = *child_to_add;
    if (parent_node->child == NULL) {
        parent_node->child = new_child;
    } else {
        ast_node *temp_child = parent_node->child;
        while (temp_child->next_child != NULL)
            temp_child = temp_child->next_child;
        temp_child->next_child = new_child;
    }
}

char* parse_tokens(char **temp_file_data, lexed_token *curr_token,
                  ast_node *curr_node) {

    *temp_file_data = lex_token(temp_file_data, &curr_token);
    if (parse_int(curr_token, curr_node)) {
        // If this is an integer, look for a valid operator.
        printf("Found integer\n");
    } else {
        // If the token was not an integer, check if it is
        // variable declaration, assignment, etc.

        // Parser High level design.
        //
        // int a := 340;
        //
        // PROGRAM
        //        `-- VARIABLE_INITIALIZED
        //            `-- INT (420) -> SYMBOL (a)

        if (strncmp_lexed_token(curr_token, "int")) {

            printf("Found variable declaration\n");
            ast_node curr_var_decl;
            curr_var_decl.type = TYPE_VAR_DECLARATION;
            curr_var_decl.child = NULL;
            curr_var_decl.next_child = NULL;
            curr_var_decl.ast_val.val = 0;
            curr_var_decl.ast_val.node_symbol = NULL;

            ast_node curr_type;
            curr_type.type = TYPE_INT;
            curr_type.child = NULL;
            curr_type.next_child = NULL;
            curr_type.ast_val.node_symbol = NULL;
            curr_type.ast_val.val = 0;

            // Lex again to look forward.
            *temp_file_data = lex_token(temp_file_data, &curr_token);
            if (strncmp_lexed_token(curr_token, ":")) {

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                ast_node curr_sym;
                curr_sym.type = TYPE_SYM;
                curr_sym.child = NULL;
                curr_sym.next_child = NULL;
                curr_sym.ast_val.val = 0;
                curr_sym.ast_val.node_symbol = NULL;

                char *symbol_str = (char *) calloc(curr_token->token_length + 1,
                                                               sizeof(char));
                CHECK_NULL(symbol_str, MEM_ERR);
                memcpy(symbol_str, curr_token->token_start,
                       curr_token->token_length);
                symbol_str[curr_token->token_length] = '\0';
                curr_sym.ast_val.node_symbol = symbol_str;

                curr_node = &curr_var_decl;

                add_ast_node_child(curr_node, &curr_type);
                add_ast_node_child(curr_node, &curr_sym);
            }
        }
    }

    // add_token_to_ll(curr_token, &root_token);
    print_ast_node(curr_node, 0);

    return *temp_file_data;
}
