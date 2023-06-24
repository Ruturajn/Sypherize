#include "../inc/parser.h"

void print_ast_node(AstNode *root_node, int indent) {
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
    AstNode *child_node = root_node->child;
    while (child_node != NULL) {
        print_ast_node(child_node, indent + 4);
        child_node = child_node->next_child;
    }
}

int parse_int(LexedToken *token, AstNode *node) {
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

Env* create_env(Env *parent_env) {
    Env *new_env = calloc(1, sizeof(Env));
    CHECK_NULL(new_env, MEM_ERR);
    new_env->parent_env = parent_env;
    new_env->binding = NULL;
    return new_env;
}

int set_env(Env *env_to_set, AstNode *identifier_node, AstNode *id_val) {
    if (env_to_set == NULL || identifier_node == NULL || id_val == NULL)
        return 0;

    IdentifierBind *temp = env_to_set->binding;

    while (temp != NULL) {
        if (node_cmp(temp->identifier, identifier_node)) {
            temp->id_val = id_val;
            // free_node(id_val); TODO
            return 1;
        }
        temp = temp->next_id_bind;
    }

    IdentifierBind *binds = calloc(1, sizeof(IdentifierBind));
    CHECK_NULL(binds, MEM_ERR);
    binds->identifier = identifier_node;
    binds->id_val = id_val;
    binds->next_id_bind = env_to_set->binding;
    env_to_set->binding = binds;
    return 1;
}

int node_cmp(AstNode *node1, AstNode *node2) {
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

AstNode *get_env(Env *env_to_get, AstNode *identifier, int* stat) {
    IdentifierBind *curr_bind = env_to_get->binding;
    while (curr_bind != NULL) {
        if (node_cmp(curr_bind->identifier, identifier)) {
            *stat = 1;
            return curr_bind->id_val;
        }
        curr_bind = curr_bind->next_id_bind;
    }
    AstNode *val = node_alloc();
    *stat = 0;
    return val;
}

void add_ast_node_child(AstNode *parent_node, AstNode *child_to_add) {
    if (parent_node == NULL || child_to_add == NULL) { return; }

    // ast_node *new_child = node_alloc();
    // *new_child = *child_to_add;
    if (parent_node->child == NULL) {
        parent_node->child = child_to_add;
    } else {
        AstNode *temp_child = parent_node->child;
        while (temp_child->next_child != NULL)
            temp_child = temp_child->next_child;
        temp_child->next_child = child_to_add;
    }
}

ParsingContext *create_parsing_context() {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *) calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, MEM_ERR);
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    AstNode *sym_node = node_symbol_create("int");
    if (!set_env(new_context->env_type, sym_node, node_int_create(49)))
        print_error("Failed to set environment", 0);
    return new_context;
}

AstNode *node_alloc() {
    AstNode *new_node = (AstNode *) calloc(1, sizeof(AstNode));
    CHECK_NULL(new_node, MEM_ERR);
    new_node->type = TYPE_NULL;
    new_node->child = NULL;
    new_node->next_child = NULL;
    new_node->ast_val.val = 0;
    new_node->ast_val.node_symbol = NULL;

    return new_node;
}

AstNode *node_symbol_create(char *symbol_str) {
    AstNode *sym_node = node_alloc();
    sym_node->type = TYPE_SYM;
    sym_node->ast_val.node_symbol = (char *)calloc(strlen(symbol_str),
                                                   sizeof(char));
    strcpy(sym_node->ast_val.node_symbol, symbol_str);

    return sym_node;
}

AstNode *node_int_create(long val) {
    AstNode *int_node = node_alloc();
    int_node->type = TYPE_INT;
    int_node->ast_val.val = val;

    return int_node;
}

void free_node(AstNode *node_to_free) {
    if (node_to_free == NULL) { return; }
    AstNode *temp = node_to_free->child;
    AstNode *temps_child = NULL;
    while (temp != NULL) {
        temps_child = temp->next_child;
        free_node(temp);
        temp = temps_child;
    }
    if (node_to_free != NULL && node_to_free->type == TYPE_SYM &&
            node_to_free->ast_val.node_symbol != NULL)
        free(node_to_free->ast_val.node_symbol);
    free(node_to_free);
}

AstNode *node_symbol_from_token_create(LexedToken *token) {

    if (token == NULL) { return NULL; }

    AstNode *node = node_alloc();
    char *symbol_str = (char *) calloc(token->token_length + 1,
                                                   sizeof(char));
    CHECK_NULL(symbol_str, MEM_ERR);
    memcpy(symbol_str, token->token_start, token->token_length);
    symbol_str[token->token_length] = '\0';
    node->ast_val.node_symbol = symbol_str;
    node->type = TYPE_SYM;
    return node;
}

char* parse_tokens(char **temp_file_data, LexedToken *curr_token,
                  AstNode *curr_node, ParsingContext *context) {

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

        AstNode *int_node = node_symbol_from_token_create(curr_token);
        int status = -1;
        int_node = get_env(context->env_type, int_node, &status);
        if (status == 0)
            print_error("Invalid TYPE", 0);
        else {
            printf("HERE\n");
            print_ast_node(int_node, 0);
        }

        if (strncmp_lexed_token(curr_token, "int")) {

            printf("Found variable declaration\n");

            AstNode *curr_var_decl = node_alloc();
            curr_var_decl->type = TYPE_VAR_DECLARATION;

            AstNode *curr_type = node_symbol_from_token_create(curr_token);
            curr_type->type = TYPE_SYM;

            // Lex again to look forward.
            *temp_file_data = lex_token(temp_file_data, &curr_token);
            if (strncmp_lexed_token(curr_token, ":")) {

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                AstNode *curr_sym = node_symbol_from_token_create(curr_token);
                curr_sym->type = TYPE_SYM;

                curr_node = curr_var_decl;

                add_ast_node_child(curr_node, curr_type);
                add_ast_node_child(curr_node, curr_sym);

                free(curr_var_decl);
            }
        }
    }

    // add_token_to_ll(curr_token, &root_token);
    print_ast_node(curr_node, 0);

    return *temp_file_data;
}
