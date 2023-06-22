#include "../inc/parser.h"

void print_ast_node(ast_node *root_node, int indent) {
    if (root_node == NULL) { return; }
    for (int i=0; i < indent; i++)
        putchar(' ');
    switch(root_node->type) {
        case TYPE_NULL:
            printf("NULL");
            break;
        case TYPE_PROGRAM:
            printf("PROGRAM");
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

int set_env(env *env_to_set, ast_node *identifier_node, ast_node *id_val) {
    if (env_to_set == NULL || identifier_node == NULL || id_val == NULL) 
        return 0;

    identifier_bind *temp = env_to_set->binding;

    while (temp != NULL) {
        if (node_cmp(temp->identifier, identifier_node)) {
            temp->id_val = id_val;
            // free_node(id_val); TODO
            return 1;
        }
        temp = temp->next_id_bind;
    }

    identifier_bind *binds = calloc(1, sizeof(identifier_bind));
    CHECK_NULL(binds, MEM_ERR);
    binds->identifier = identifier_node;
    binds->id_val = id_val;
    binds->next_id_bind = env_to_set->binding;
    env_to_set->binding = binds;
    return 1;
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
        case TYPE_PROGRAM:
            if (node2->type == TYPE_PROGRAM)
                return 1;
            break;
        case TYPE_ROOT:
            printf("Compare Programs : Not implemented\n");
            break;
        case TYPE_INT:
            if (node1->ast_val.val == node2->ast_val.val)
                return 1;
            break;
        case TYPE_BINARY_OPERATOR:
            if (node2->type == TYPE_BINARY_OPERATOR) {
                printf("TODO : BINARY OPERATOR!\n");
                return 1;
            }
            break;
        case TYPE_VAR_DECLARATION:
            if (node2->type == TYPE_VAR_DECLARATION) {
                printf("TODO : VAR DECLARATION!\n");
                return 1;
            }
        case TYPE_VAR_INIT:
            if (node2->type == TYPE_VAR_INIT) {
                printf("TODO : VAR INIT!\n");
                return 1;
            }
        case TYPE_SYM:
            if (node1->ast_val.node_symbol != NULL &&
                node2->ast_val.node_symbol != NULL &&
                (strcmp(node1->ast_val.node_symbol, node2->ast_val.node_symbol) == 0))
                return 1;
            else if (node1->ast_val.node_symbol == NULL &&
                     node2->ast_val.node_symbol == NULL)
                return 1;
        default:
            break;
    }

    return 0;
}

ast_node *get_env(env *env_to_get, ast_node *identifier) {
    identifier_bind *curr_bind = env_to_get->binding;
    while (curr_bind != NULL) {
        if (node_cmp(curr_bind->identifier, identifier))
            return curr_bind->id_val;
        curr_bind = curr_bind->next_id_bind;
    }
    ast_node *val = node_alloc();
    return val;
}

void add_ast_node_child(ast_node *parent_node, ast_node *child_to_add) {
    if (parent_node == NULL || child_to_add == NULL) { return; }

    // ast_node *new_child = node_alloc();
    // *new_child = *child_to_add;
    if (parent_node->child == NULL) {
        parent_node->child = child_to_add;
    } else {
        ast_node *temp_child = parent_node->child;
        while (temp_child->next_child != NULL)
            temp_child = temp_child->next_child;
        temp_child->next_child = child_to_add;
    }
}

parsing_context *create_parsing_context() {
    parsing_context *new_context = NULL;
    new_context = (parsing_context *) calloc(1, sizeof(parsing_context));
    CHECK_NULL(new_context, MEM_ERR);
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    if (!set_env(new_context->env_type, node_symbol_create("int"), node_int_create(49)))
        print_error("Failed to set environment");
    return new_context;
}

ast_node *node_alloc() {
    ast_node *new_node = (ast_node *) calloc(1, sizeof(ast_node));
    CHECK_NULL(new_node, MEM_ERR);
    new_node->type = TYPE_NULL;
    new_node->child = NULL;
    new_node->next_child = NULL;
    new_node->ast_val.val = 0;
    new_node->ast_val.node_symbol = NULL;

    return new_node;
}

ast_node *node_symbol_create(char *symbol_str) {
    ast_node *sym_node = node_alloc();
    sym_node->type = TYPE_SYM;
    sym_node->ast_val.node_symbol = (char *)calloc(strlen(symbol_str),
                                                   sizeof(char));
    strcpy(sym_node->ast_val.node_symbol, symbol_str);

    return sym_node;
}

ast_node *node_int_create(long val) {
    ast_node *int_node = node_alloc();
    int_node->type = TYPE_INT;
    int_node->ast_val.val = val;

    return int_node;
}

// void free_node(ast_node *node_to_free) {
//     ast_node *temp = node_to_free;
//     while (temp->child != NULL) {
//
//     }
//     free(temp->ast_val.node_symbol);
//     free(temp);
// }

ast_node *node_symbol_from_token_create(enum node_type type, lexed_token *token) {

    if (token == NULL) { return NULL; }

    ast_node *node = node_alloc();
    node->type = type;
    char *symbol_str = (char *) calloc(token->token_length + 1,
                                                   sizeof(char));
    CHECK_NULL(symbol_str, MEM_ERR);
    memcpy(symbol_str, token->token_start, token->token_length);
    symbol_str[token->token_length] = '\0';
    node->ast_val.node_symbol = symbol_str;
    return node;
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

            ast_node *curr_var_decl = node_alloc();
            curr_var_decl->type = TYPE_VAR_DECLARATION;

            ast_node *curr_type = node_symbol_from_token_create(TYPE_SYM,
                                                          curr_token);

            // Lex again to look forward.
            *temp_file_data = lex_token(temp_file_data, &curr_token);
            if (strncmp_lexed_token(curr_token, ":")) {

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                ast_node *curr_sym = node_symbol_from_token_create(TYPE_SYM,
                                                                   curr_token);

                curr_node = curr_var_decl;

                add_ast_node_child(curr_node, curr_type);
                add_ast_node_child(curr_node, curr_sym);
            }
        }
    }

    // add_token_to_ll(curr_token, &root_token);
    print_ast_node(curr_node, 0);

    return *temp_file_data;
}
