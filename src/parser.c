#include "../inc/parser.h"

void print_ast_node(AstNode *root_node, int indent) {
    if (root_node == NULL) {
        return;
    }
    for (int i = 0; i < indent; i++)
        putchar(' ');
    switch (root_node->type) {
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
    case TYPE_VAR_REASSIGNMENT:
        printf("VAR REASSIGNMENT");
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
    if (token == NULL || node == NULL) {
        return 2;
    }
    if (token->token_length == 0 && *(token->token_start) == '0') {
        node->type = TYPE_INT;
        node->ast_val.val = 0;
    } else {
        long temp = strtol(token->token_start, NULL, 10);
        if (temp != 0) {
            node->type = TYPE_INT;
            node->ast_val.val = temp;
        } else {
            return 0;
        }
    }
    return 1;
}

Env *create_env(Env *parent_env) {
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
    if ((node1 == NULL && node2 != NULL) || (node1 != NULL && node2 == NULL))
        return 0;

    if (node1->type != node2->type) {
        return 0;
    }

    switch (node1->type) {
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
            (strcmp(node1->ast_val.node_symbol, node2->ast_val.node_symbol) ==
             0))
            return 1;
        else if (node1->ast_val.node_symbol == NULL &&
                 node2->ast_val.node_symbol == NULL)
            return 1;
    case TYPE_VAR_REASSIGNMENT:
        if (node2->type == TYPE_VAR_REASSIGNMENT) {
            printf("TODO : VAR REASSIGNMENT!\n");
            return 1;
        }
    default:
        break;
    }

    return 0;
}

AstNode *get_env(Env *env_to_get, AstNode *identifier, int *stat) {
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
    if (parent_node == NULL || child_to_add == NULL) {
        return;
    }
    if (parent_node->child == NULL) {
        AstNode *new_node = node_alloc();
        *new_node = *child_to_add;
        parent_node->child = new_node;
    } else {
        AstNode *temp_child = parent_node->child;
        while (temp_child->child != NULL)
            temp_child = temp_child->child;
        while (temp_child->next_child != NULL)
            temp_child = temp_child->next_child;
        AstNode *new_node = node_alloc();
        *new_node = *child_to_add;
        temp_child->next_child = new_node;
    }
}

ParsingContext *create_parsing_context() {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *)calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, MEM_ERR);
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    AstNode *sym_node = node_symbol_create("int");
    if (!set_env(new_context->env_type, sym_node, node_int_create(49)))
        print_error("Failed to set environment", 0);
    return new_context;
}

AstNode *node_alloc() {
    AstNode *new_node = (AstNode *)calloc(1, sizeof(AstNode));
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
    sym_node->ast_val.node_symbol =
        (char *)calloc(strlen(symbol_str), sizeof(char));
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
    if (node_to_free == NULL) {
        return;
    }
    AstNode *temp = node_to_free->child;
    while (temp != NULL) {
        free_node(temp);
        temp = temp->child;
    }

    AstNode *temp1 = NULL;
    while (temp != NULL) {
        temp1 = temp->next_child;
        if (temp->type == TYPE_SYM && temp->ast_val.node_symbol != NULL)
            free(temp->ast_val.node_symbol);
        free(temp);
        temp = temp1;
    }
}

AstNode *node_symbol_from_token_create(LexedToken *token) {

    if (token == NULL) {
        return NULL;
    }

    AstNode *node = node_alloc();
    char *symbol_str = (char *)calloc(token->token_length + 1, sizeof(char));
    CHECK_NULL(symbol_str, MEM_ERR);
    memcpy(symbol_str, token->token_start, token->token_length);
    symbol_str[token->token_length] = '\0';
    node->ast_val.node_symbol = symbol_str;
    node->type = TYPE_SYM;
    return node;
}

int copy_node(AstNode *dst_node, AstNode *src_node) {
    if (src_node == NULL || dst_node == NULL) {
        return 0;
    }

    dst_node->type = src_node->type;
    dst_node->ast_val.val = src_node->ast_val.val;

    if (src_node->type == TYPE_SYM)
        dst_node->ast_val.node_symbol = strdup(src_node->ast_val.node_symbol);
    else
        dst_node->ast_val.node_symbol = NULL;

    AstNode *temp_child = src_node->child;
    AstNode *temp_dst_child = NULL;
    while (temp_child != NULL) {
        // Allocate memory for a new child node.
        AstNode *new_child = node_alloc();

        /**
         *  If temp_dst_child is NULL, it means we
         *  are copying the first child, in which case
         *  the dst_node's child is new_child, and we
         *  can set temp_dst_child to new_child.
         *  On the other hand, when temp_dst_child is not
         *  NULL, we can set it's next child to be the new_child,
         *  and move temp_dst_child forward, by setting it to
         *  its next child.
         */
        if (temp_dst_child != NULL) {
            temp_dst_child->next_child = new_child;
            // temp_dst_child = temp_dst_child->next_child;
        } else {
            dst_node->child = new_child;
            // temp_dst_child = new_child;
        }
        temp_dst_child = new_child;

        copy_node(temp_dst_child, temp_child);
        temp_child = temp_child->next_child;
    }

    return 0;
}

char *parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode **curr_expr, ParsingContext *context) {

    *temp_file_data = lex_token(temp_file_data, &curr_token);

    if (curr_token == NULL)
        return *temp_file_data;

    if (strncmp_lexed_token(curr_token, ";"))
        return *temp_file_data;

    // Check if the current token is an integer.
    if (parse_int(curr_token, *curr_expr)) {
        // If this is an integer, look for a valid operator.
        printf("Found integer\n");
        return *temp_file_data;
    }
    // If the token was not an integer, check if it is
    // variable declaration, assignment, etc.

    /**
     * Parser High level design.
     *
     * int a := 340;
     *
     * PROGRAM
     *        `-- VARIABLE_INITIALIZED
     *            `-- INT (420) -> SYMBOL (a)
     */

    AstNode *sym_node = node_symbol_from_token_create(curr_token);
    int status = -1;
    sym_node = get_env(context->env_type, sym_node, &status);
    if (status == 0)
        print_error("Invalid TYPE", 0);

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

            add_ast_node_child(curr_var_decl, curr_sym);

            // Lex again to look forward.
            *temp_file_data = lex_token(temp_file_data, &curr_token);
            if (strncmp_lexed_token(curr_token, ":")) {
                *temp_file_data = lex_token(temp_file_data, &curr_token);

                if (strncmp_lexed_token(curr_token, "=")) {
                    AstNode *new_expr = node_alloc();
                    *temp_file_data = parse_tokens(temp_file_data, curr_token,
                                                   &new_expr, context);
                    add_ast_node_child(curr_var_decl, new_expr);

                    if (new_expr->type != sym_node->type)
                        print_error("Mismatched TYPE: ", 0);

                    curr_type->ast_val.val = new_expr->ast_val.val;

                    *curr_expr = curr_var_decl;

                    AstNode *sym_node = node_alloc();
                    copy_node(sym_node, curr_sym);
                    if (!set_env(context->vars, sym_node, curr_type)) {
                        print_error(
                            "Unable to set environment binding for variable",
                            0);
                    }

                    return *temp_file_data;
                }
            }
        }
    } else {
        // Lex again to look forward.
        *temp_file_data = lex_token(temp_file_data, &curr_token);
        if (strncmp_lexed_token(curr_token, ":")) {

            AstNode *var_bind = node_alloc();
            var_bind = get_env(context->vars, sym_node, &status);
            if (status && var_bind != NULL) {
                // re-assignment or redefinition (which is an error),
                // otherwise invalid syntax error.

                // Lex again to look forward.
                *temp_file_data = lex_token(temp_file_data, &curr_token);
                if (strncmp_lexed_token(curr_token, "=")) {
                    AstNode *new_expr = node_alloc();
                    *temp_file_data = parse_tokens(temp_file_data, curr_token,
                                                   &new_expr, context);
                    if (new_expr->type != var_bind->child->type)
                        print_error("Mismatched TYPE", 0);

                    AstNode *node_reassign = node_alloc();
                    node_reassign->type = TYPE_VAR_REASSIGNMENT;
                    add_ast_node_child(node_reassign, new_expr);

                    *curr_expr = node_reassign;

                    // new_expr->next_child = var_bind->child->next_child;
                    // set_env(context->vars, sym_node, new_expr);
                    // var_bind->child = new_expr;

                    return *temp_file_data;
                }
                print_error("UKNOWN", 0);
            }
        }
        print_error("Redefinition of a variable", 0);
    }
    return *temp_file_data;
}
