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
    case TYPE_FUNCTION:
        printf("FUNCTION");
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

int set_env(Env **env_to_set, AstNode *identifier_node, AstNode *id_val) {
    if (env_to_set == NULL || identifier_node == NULL || id_val == NULL)
        return 0;

    IdentifierBind *temp = (*env_to_set)->binding;

    IdentifierBind *binds = calloc(1, sizeof(IdentifierBind));
    CHECK_NULL(binds, MEM_ERR);
    binds->identifier = node_alloc();
    copy_node(binds->identifier, identifier_node);
    binds->id_val = node_alloc();
    copy_node(binds->id_val, id_val);

    if (temp == NULL) {
        (*env_to_set)->binding = binds;
        return 1;
    }

    while (temp != NULL) {
        if (node_cmp(temp->identifier, identifier_node)) {
            copy_node(temp->id_val, id_val);
            return 1;
        }
        temp = temp->next_id_bind;
    }

    binds->next_id_bind = (*env_to_set)->binding;
    (*env_to_set)->binding = binds;
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
        break;
    case TYPE_PROGRAM:
        break;
    case TYPE_ROOT:
        printf("Compare Programs : Not implemented\n");
        break;
    case TYPE_INT:
        if (node1->ast_val.val == node2->ast_val.val)
            return 1;
        break;
    case TYPE_BINARY_OPERATOR:
        printf("TODO : BINARY OPERATOR!\n");
        break;
    case TYPE_VAR_DECLARATION:
        printf("TODO : VAR DECLARATION!\n");
        break;
    case TYPE_VAR_INIT:
        printf("TODO : VAR INIT!\n");
        break;
    case TYPE_SYM:
        if (node1->ast_val.node_symbol != NULL &&
            node2->ast_val.node_symbol != NULL &&
            (strcmp(node1->ast_val.node_symbol, node2->ast_val.node_symbol) ==
             0))
            return 1;
        else if (node1->ast_val.node_symbol == NULL &&
                 node2->ast_val.node_symbol == NULL)
            return 1;
        break;
    case TYPE_VAR_REASSIGNMENT:
        printf("TODO : VAR REASSIGNMENT!\n");
        break;
    case TYPE_FUNCTION:
        printf("TODO : FUNCTION!\n");
        break;
    default:
        break;
    }

    return 0;
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

void add_ast_node_child(AstNode *parent_node, AstNode *child_to_add) {
    if (parent_node == NULL || child_to_add == NULL)
        return;
    if (parent_node->child == NULL) {
        parent_node->child = child_to_add;
        return;
    }
    AstNode *temp_child = parent_node->child;
    while (temp_child->next_child != NULL) {
        temp_child = temp_child->next_child;
    }
    temp_child->next_child = child_to_add;
}

ParsingContext *create_parsing_context(ParsingContext *parent_ctx) {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *)calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, MEM_ERR);
    new_context->op = NULL;
    new_context->res = NULL;
    new_context->parent_ctx = parent_ctx;
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    new_context->funcs = create_env(NULL);
    return new_context;
}

ParsingContext *create_default_parsing_context() {
    ParsingContext *new_context = create_parsing_context(NULL);
    AstNode *sym_node = create_node_symbol("int");
    ast_add_type_node(&new_context->env_type, TYPE_INT, sym_node, sizeof(long));
    return new_context;
}

AstNode *node_alloc() {
    AstNode *new_node = (AstNode *)calloc(1, sizeof(AstNode));
    CHECK_NULL(new_node, MEM_ERR);
    new_node->type = TYPE_NULL;
    new_node->child = NULL;
    new_node->next_child = NULL;
    new_node->ast_val.node_symbol = NULL;
    new_node->ast_val.val = 0;

    return new_node;
}

AstNode *create_node_symbol(char *symbol_str) {
    AstNode *sym_node = node_alloc();
    sym_node->type = TYPE_SYM;
    sym_node->ast_val.node_symbol =
        (char *)calloc(strlen(symbol_str), sizeof(char));
    strcpy(sym_node->ast_val.node_symbol, symbol_str);

    return sym_node;
}

AstNode *create_node_none() {
    AstNode *null_node = node_alloc();
    null_node->type = TYPE_NULL;

    return null_node;
}

AstNode *create_node_int(long val) {
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
    if (src_node == NULL || dst_node == NULL)
        return 0;

    dst_node->type = src_node->type;

    if (src_node->type == TYPE_SYM)
        dst_node->ast_val.node_symbol = strdup(src_node->ast_val.node_symbol);
    else
        dst_node->ast_val.node_symbol = NULL;

    dst_node->ast_val.val = src_node->ast_val.val;

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
        if (temp_dst_child != NULL)
            temp_dst_child->next_child = new_child;
        else
            dst_node->child = new_child;

        temp_dst_child = new_child;

        copy_node(temp_dst_child, temp_child);
        temp_child = temp_child->next_child;
    }
    return 0;
}

// TODO: Add more types as the language starts supporting
// new types.
int is_known_type(LexedToken *token, int *type) {
    if (token == NULL) {
        *type = TYPE_NULL;
        return 0;
    }
    if (strncmp_lexed_token(token, "int")) {
        *type = TYPE_INT;
        return 1;
    }
    *type = TYPE_NULL;
    return 0;
}

int check_next_token(char *string_to_cmp, char **temp_file_data,
                     LexedToken **token) {
    if (string_to_cmp == NULL || *temp_file_data == NULL || token == NULL) {
        print_error("NULL pointer passed to check_next_token()", 0, NULL);
        return 0;
    }
    char *prev_file_data = *temp_file_data;
    LexedToken *temp_token = *token;
    prev_file_data = lex_token(&prev_file_data, &temp_token);
    if (strncmp_lexed_token(temp_token, string_to_cmp)) {
        *token = temp_token;
        *temp_file_data = prev_file_data;
        return 1;
    }
    return 0;
}

void ast_add_type_node(Env **env_type, int node_type, AstNode *sym,
                       long byte_size) {
    if (sym == NULL || byte_size <= 0)
        print_error("Unable to add a new type to the types environment", 1,
                    NULL);

    AstNode *sz_node = node_alloc();
    sz_node->type = TYPE_INT;
    sz_node->ast_val.val = byte_size;

    AstNode *new_type_node = node_alloc();
    new_type_node->type = node_type;
    new_type_node->child = sz_node;

    if (!set_env(env_type, sym, new_type_node)) {
        printf("Type redefinition : `%s`\n", sym->ast_val.node_symbol);
        print_error("Unable to redefine type", 1, NULL);
    }
}

char *parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode **curr_expr, ParsingContext **context) {

    AstNode *running_expr = *curr_expr;

    while ((*temp_file_data = lex_token(temp_file_data, &curr_token)) &&
           **temp_file_data != '\0') {

        if (curr_token == NULL)
            return *temp_file_data;

        if (strncmp_lexed_token(curr_token, ";")) {
            if ((*context)->parent_ctx != NULL) {
                if (check_next_token("}", temp_file_data, &curr_token)) {
                    running_expr = (*context)->res;
                    *context = (*context)->parent_ctx;
                    print_ast_node(running_expr, 0);
                    break;
                }
                print_ast_node((*context)->res, 0);
                (*context)->res->next_child = node_alloc();
                (*context)->res = (*context)->res->next_child;
                running_expr = (*context)->res;
                continue;
            }
            return *temp_file_data;
        }

        // Check if the current token is an integer.
        if (parse_int(curr_token, running_expr)) {
            // If this is an integer, look for a valid operator.
            // return *temp_file_data;
        } else {
            // If the token was not an integer, check if it is
            // variable declaration, assignment, etc.

            if (strncmp_lexed_token(curr_token, "def_func")) {
                CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);
                /**
                 * FUNCTION
                 *         `-- LIST OF PARAMETERS
                 *         |                     `--PARAMETER
                 *         |                                 `-- SYM: NAME
                 *         |                                 `-- SYM: TYPE
                 *         `-- RETURN TYPE SYMBOL
                 *         `-- PROGRAM / LIST OF EXPRESSIONS
                 */
                *temp_file_data = lex_token(temp_file_data, &curr_token);
                running_expr->type = TYPE_FUNCTION;
                AstNode *func_name = node_symbol_from_token_create(curr_token);
                // Function name should be put into the parsing context, not
                // into the AST.

                if (!check_next_token("(", temp_file_data, &curr_token))
                    print_error(SYNTAX_ERR, 1, curr_token);

                AstNode *param_list = node_alloc();
                for (;;) {
                    if (check_next_token(")", temp_file_data, &curr_token))
                        break;

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_type =
                        node_symbol_from_token_create(curr_token);

                    if (!check_next_token(":", temp_file_data, &curr_token))
                        print_error(SYNTAX_ERR, 1, curr_token);

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_name =
                        node_symbol_from_token_create(curr_token);

                    AstNode *param = node_alloc();

                    add_ast_node_child(param, param_name);
                    add_ast_node_child(param, param_type);

                    add_ast_node_child(param_list, param);

                    if (!check_next_token(",", temp_file_data, &curr_token)) {
                        if (check_next_token(")", temp_file_data, &curr_token))
                            break;
                        print_error(SYNTAX_ERR, 1, curr_token);
                    }
                }
                CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);

                if (!check_next_token("~", temp_file_data, &curr_token))
                    print_error(SYNTAX_ERR, 1, curr_token);

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                AstNode *return_type =
                    node_symbol_from_token_create(curr_token);

                add_ast_node_child(running_expr, param_list);
                add_ast_node_child(running_expr, return_type);

                if (!check_next_token("{", temp_file_data, &curr_token))
                    print_error("Function definition doesn't have a body", 1,
                                curr_token);

                *context = create_parsing_context(*context);
                (*context)->op = create_node_symbol("def_func");

                AstNode *params = running_expr->child->child;
                while (params != NULL) {
                    if (!set_env(&((*context)->vars), params->child,
                                 params->child->next_child)) {
                        print_error("Unable to set environment binding for "
                                    "variable",
                                    0, NULL);
                    }
                    params = params->next_child;
                }

                AstNode *func_body = node_alloc();
                AstNode *func_expr = node_alloc();
                add_ast_node_child(func_body, func_expr);
                add_ast_node_child(running_expr, func_body);

                (*context)->res = func_expr;

                if (!set_env(&((*context)->funcs), func_name, running_expr)) {
                    print_error("Unable to set environment binding for "
                                "variable",
                                0, NULL);
                }

                running_expr = func_expr;
                continue;
            } else {
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
                AstNode *res = parser_get_type(*context, sym_node, &status);
                if (status) {
                    AstNode *curr_var_decl = node_alloc();
                    curr_var_decl->type = TYPE_VAR_DECLARATION;

                    AstNode *curr_type =
                        node_symbol_from_token_create(curr_token);
                    curr_type->type = res->type;

                    // Lex again to look forward.
                    if (check_next_token(":", temp_file_data, &curr_token)) {

                        CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);

                        *temp_file_data =
                            lex_token(temp_file_data, &curr_token);

                        AstNode *curr_sym =
                            node_symbol_from_token_create(curr_token);
                        curr_sym->type = TYPE_SYM;

                        get_env((*context)->vars, curr_sym, &status);
                        if (status)
                            print_error("Redefinition of a variable", 1,
                                        curr_token);

                        add_ast_node_child(curr_var_decl, curr_sym);

                        // Lex again to look forward.
                        if (check_next_token(":", temp_file_data,
                                             &curr_token)) {
                            CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);
                            if (check_next_token("=", temp_file_data,
                                                 &curr_token)) {
                                CHECK_END(**temp_file_data, SYNTAX_ERR,
                                          curr_token);

                                AstNode *new_expr = node_alloc();
                                add_ast_node_child(curr_var_decl, new_expr);
                                AstNode *sym_name = node_alloc();
                                copy_node(sym_name, curr_sym);
                                if (!set_env(&((*context)->vars), sym_name,
                                             sym_node)) {
                                    print_error(
                                        "Unable to set environment binding for "
                                        "variable",
                                        0, NULL);
                                }
                                *running_expr = *curr_var_decl;
                                running_expr = new_expr;
                                continue;
                            }
                        }
                        // If the control flow is here, it means that it is a
                        // variable declaration without intiialization.

                        // Initialize the value of the variable to NULL.
                        AstNode *sym_name_node = node_alloc();
                        copy_node(sym_name_node, curr_sym);

                        AstNode *init_val = create_node_none();
                        // init_val->type = res->type;

                        add_ast_node_child(curr_var_decl, init_val);

                        if (!set_env(&((*context)->vars), sym_name_node,
                                     sym_node)) {
                            print_error("Unable to set environment binding for "
                                        "variable",
                                        0, NULL);
                        }
                        *running_expr = *curr_var_decl;
                    } else {
                        *temp_file_data =
                            lex_token(temp_file_data, &curr_token);
                        print_error(SYNTAX_ERR, 1, 0);
                        print_lexed_token(curr_token);
                    }
                    return *temp_file_data;
                }
                free_node(res);

                // Lex again to look forward.
                if (check_next_token(":", temp_file_data, &curr_token)) {
                    CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);

                    AstNode *var_bind =
                        get_env((*context)->vars, sym_node, &status);
                    if (status) {
                        // re-assignment or redefinition (which is an error),
                        // otherwise invalid syntax error.

                        // Lex again to look forward.
                        if (check_next_token("=", temp_file_data,
                                             &curr_token)) {
                            CHECK_END(**temp_file_data, SYNTAX_ERR, curr_token);

                            AstNode *new_expr = node_alloc();
                            AstNode *node_reassign = node_alloc();
                            node_reassign->type = TYPE_VAR_REASSIGNMENT;

                            add_ast_node_child(node_reassign, sym_node);
                            add_ast_node_child(node_reassign, new_expr);

                            *running_expr = *node_reassign;
                            running_expr = new_expr;
                            continue;

                        } else {
                            *temp_file_data =
                                lex_token(temp_file_data, &curr_token);
                            print_error("UKNOWN : ", 1, curr_token);
                        }
                    } else
                        print_error("Undefined Symbol", 1, curr_token);
                    free_node(var_bind);
                    return *temp_file_data;
                } else {
                    if ((*context)->parent_ctx == NULL)
                        break;

                    AstNode *op = (*context)->op;
                    if (op->type != TYPE_SYM)
                        print_error("Compiler - Context operator not a symbol",
                                    1, NULL);

                    if (strcmp(op->ast_val.node_symbol, "def_func") == 0) {
                        if (strncmp_lexed_token(curr_token, "}") ||
                            check_next_token("}", temp_file_data,
                                             &curr_token)) {
                            *context = (*context)->parent_ctx;
                            break;
                        }
                    }
                }
            }
        }
    }
    return *temp_file_data;
}
