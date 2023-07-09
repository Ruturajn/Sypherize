#include "../inc/parser.h"
#include "../inc/ast_funcs.h"

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

ParsingContext *create_parsing_context(ParsingContext *parent_ctx) {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *)calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, "Unable to allocate memory for new parsing context",
               NULL);
    new_context->op = NULL;
    new_context->res = NULL;
    new_context->parent_ctx = parent_ctx;
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    new_context->funcs = create_env(NULL);
    new_context->binary_ops = create_env(NULL);
    return new_context;
}

ParsingContext *create_default_parsing_context() {
    ParsingContext *new_context = create_parsing_context(NULL);
    AstNode *sym_node = create_node_symbol("int");
    ast_add_type_node(&new_context->env_type, TYPE_INT, sym_node, sizeof(long));
    ast_add_binary_ops(&new_context, "+", 5, "int", "int", "int");
    ast_add_binary_ops(&new_context, "-", 5, "int", "int", "int");
    ast_add_binary_ops(&new_context, "*", 10, "int", "int", "int");
    ast_add_binary_ops(&new_context, "/", 10, "int", "int", "int");
    return new_context;
}

void ast_add_type_node(Env **env_type, int node_type, AstNode *sym,
                       long byte_size) {
    if (sym == NULL || byte_size <= 0)
        print_error(ERR_COMMON,
                    "Unable to add a new type to the types environment", NULL,
                    0);

    AstNode *sz_node = node_alloc();
    sz_node->type = TYPE_INT;
    sz_node->ast_val.val = byte_size;

    AstNode *new_type_node = node_alloc();
    new_type_node->type = node_type;
    new_type_node->child = sz_node;

    if (!set_env(env_type, sym, new_type_node)) {
        print_error(ERR_REDEFINITION, "Unable to redefine type : `%s`",
                    sym->ast_val.node_symbol, 0);
    }
}

void ast_add_binary_ops(ParsingContext **context, char *bin_op, int precedence,
                        char *ret_type, char *lhs_type, char *rhs_type) {
    AstNode *node_sym_op = create_node_symbol(bin_op);
    AstNode *node_bin_op_body = node_alloc();
    AstNode *node_prec = create_node_int(precedence);
    AstNode *node_ret = create_node_symbol(ret_type);
    AstNode *node_lhs = create_node_symbol(lhs_type);
    AstNode *node_rhs = create_node_symbol(rhs_type);

    add_ast_node_child(node_bin_op_body, node_prec);
    add_ast_node_child(node_bin_op_body, node_ret);
    add_ast_node_child(node_bin_op_body, node_lhs);
    add_ast_node_child(node_bin_op_body, node_rhs);

    ParsingContext *temp = *context;

    while (temp->parent_ctx != NULL)
        temp = temp->parent_ctx;

    if (!set_env(&((temp)->binary_ops), node_sym_op, node_bin_op_body)) {
        print_error(ERR_COMMON,
                    "Unable to set environment binding for "
                    "binary operator : `%s`",
                    node_sym_op->ast_val.node_symbol, 0);
    }
}

int parse_binary_infix_op(char **temp_file_data, LexedToken **curr_token,
                          ParsingContext **context, long *running_precedence,
                          AstNode **curr_expr, AstNode **running_expr) {
    char *tmp_data = *temp_file_data;
    LexedToken *tmp_token = NULL;
    tmp_data = lex_token(&tmp_data, &tmp_token);
    AstNode *node_binary_op = node_symbol_from_token_create(tmp_token);
    int stat = -1;
    ParsingContext *global_ctx = *context;
    while (global_ctx->parent_ctx != NULL)
        global_ctx = global_ctx->parent_ctx;
    AstNode *bin_op_val =
        get_env(global_ctx->binary_ops, node_binary_op, &stat);
    if (stat) {
        *temp_file_data = tmp_data;
        **curr_token = *tmp_token;
        long temp_prec = bin_op_val->child->ast_val.val;
        AstNode *node_bin_op_body = node_alloc();
        node_bin_op_body->type = TYPE_BINARY_OPERATOR;
        if (temp_prec <= *running_precedence) {
            // `curr_expr` needs to change completely,
            // and it needs to become a child of this new node, because
            // we encountered something of lower precedence, and hence
            // this node will have a binary operator node as a child.
            AstNode *temp_expr_copy = node_alloc();
            copy_node(temp_expr_copy, *curr_expr);
            add_ast_node_child(node_bin_op_body, temp_expr_copy);
            node_bin_op_body->ast_val.node_symbol =
                strdup(node_binary_op->ast_val.node_symbol);
            node_bin_op_body->next_child = NULL;

            AstNode *rhs_node = node_alloc();
            add_ast_node_child(node_bin_op_body, rhs_node);

            **curr_expr = *node_bin_op_body;
            **running_expr = **curr_expr;
            *running_expr = rhs_node;
        } else {
            // Here `running_expr` needs to change, and will store the
            // value of the next integer that needs to be used for the
            // operation based on the operator.
            AstNode *temp_expr_copy = node_alloc();
            copy_node(temp_expr_copy, *running_expr);
            add_ast_node_child(node_bin_op_body, temp_expr_copy);
            node_bin_op_body->ast_val.node_symbol =
                strdup(node_binary_op->ast_val.node_symbol);
            node_bin_op_body->next_child = NULL;

            AstNode *rhs_node = node_alloc();
            add_ast_node_child(node_bin_op_body, rhs_node);

            **running_expr = *node_bin_op_body;
            *running_expr = rhs_node;
        }
        *running_precedence = temp_prec;
        return 1;
    }
    free(bin_op_val);
    free(tmp_token);
    free(node_binary_op);
    return 0;
}

void lex_and_parse(char *file_dest, ParsingContext **curr_context,
                   AstNode **program) {

    char *file_data = read_file_data(file_dest);

    char *temp_file_data = file_data;
    temp_file_data += strspn(temp_file_data, WHITESPACE);

    LexedToken *root_token = NULL;
    LexedToken *curr_token = root_token;

    // The whole program will be tokenized and parsed into an
    // AST which will be created from the curr_node.
    AstNode *curr_expr;

    while (*temp_file_data != '\0') {
        curr_expr = node_alloc();

        temp_file_data =
            parse_tokens(&temp_file_data, curr_token, &curr_expr, curr_context);
        if (curr_expr->type != TYPE_NULL)
            add_ast_node_child(*program, curr_expr);

        free_node(curr_expr);
    }

    print_ast_node(*program, 0);
    free(file_data);
}

char *parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode **curr_expr, ParsingContext **context) {

    AstNode *running_expr = *curr_expr;
    long running_precedence = 0;
    for (;;) {
        *temp_file_data = lex_token(temp_file_data, &curr_token);

        if (curr_token == NULL)
            return *temp_file_data;

        // Check if the current token is an integer.
        if (parse_int(curr_token, running_expr)) {
            // If this is an integer, look for a valid operator.
            // return *temp_file_data;
        } else {
            // If the token was not an integer, check if it is
            // variable declaration, assignment, etc.

            if (strncmp_lexed_token(curr_token, "def_func")) {
                CHECK_END(**temp_file_data, "End of file after : `def_func`",
                          NULL);
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
                AstNode *func_node = node_alloc();
                func_node->type = TYPE_FUNCTION;
                AstNode *func_name = node_symbol_from_token_create(curr_token);
                // Function name should be put into the parsing context, not
                // into the AST.

                if (!check_next_token("(", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX,
                                "Invalid function definition for : `%s`",
                                func_name->ast_val.node_symbol, 0);

                AstNode *param_list = node_alloc();
                for (;;) {
                    if (check_next_token(")", temp_file_data, &curr_token))
                        break;

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_type =
                        node_symbol_from_token_create(curr_token);

                    if (!check_next_token(":", temp_file_data, &curr_token))
                        print_error(
                            ERR_SYNTAX,
                            "Couldn't find `:` after parameter type : `%s`",
                            param_type->ast_val.node_symbol, 0);

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
                        print_error(
                            ERR_SYNTAX,
                            "Could not find end of function definition : `%s`",
                            func_name->ast_val.node_symbol, 0);
                    }
                }
                CHECK_END(**temp_file_data,
                          "End of file - Incomplete function definition : `%s`",
                          func_name->ast_val.node_symbol);

                if (!check_next_token("~", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX,
                                "Return type not specified for function : `%s`",
                                func_name->ast_val.node_symbol, 0);

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                AstNode *return_type =
                    node_symbol_from_token_create(curr_token);

                add_ast_node_child(func_node, param_list);
                add_ast_node_child(func_node, return_type);

                if (!set_env(&((*context)->funcs), func_name, func_node)) {
                    print_error(ERR_COMMON,
                                "Unable to set environment binding for "
                                "function : `%s`",
                                func_name->ast_val.node_symbol, 0);
                }

                if (!check_next_token("{", temp_file_data, &curr_token))
                    print_error(
                        ERR_SYNTAX,
                        "Function definition doesn't have a body : `%s`",
                        func_name->ast_val.node_symbol, 0);

                *context = create_parsing_context(*context);
                (*context)->op = create_node_symbol("def_func");

                AstNode *params = func_node->child->child;
                while (params != NULL) {
                    if (!set_env(&((*context)->vars), params->child,
                                 params->child->next_child)) {
                        print_error(ERR_COMMON,
                                    "Unable to set environment binding for "
                                    "variable : `%s`",
                                    params->child->ast_val.node_symbol, 0);
                    }
                    params = params->next_child;
                }

                AstNode *func_body = node_alloc();
                AstNode *func_expr = node_alloc();
                add_ast_node_child(func_body, func_expr);
                add_ast_node_child(func_node, func_body);

                (*context)->res = func_expr;
                *running_expr = *func_node;
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
                        CHECK_END(**temp_file_data,
                                  "End of file during variable declaration",
                                  NULL);

                        *temp_file_data =
                            lex_token(temp_file_data, &curr_token);

                        AstNode *curr_sym =
                            node_symbol_from_token_create(curr_token);
                        curr_sym->type = TYPE_SYM;

                        get_env((*context)->vars, curr_sym, &status);
                        if (status)
                            print_error(ERR_REDEFINITION,
                                        "Redefinition of variable : `%s`",
                                        curr_sym->ast_val.node_symbol, 0);

                        add_ast_node_child(curr_var_decl, curr_sym);

                        // Lex again to look forward.
                        if (check_next_token(":", temp_file_data,
                                             &curr_token)) {
                            CHECK_END(**temp_file_data,
                                      "End of file during variable declaration "
                                      ": `%s`",
                                      curr_sym->ast_val.node_symbol);
                            if (check_next_token("=", temp_file_data,
                                                 &curr_token)) {
                                CHECK_END(**temp_file_data,
                                          "End of file during variable "
                                          "declaration : `%s`",
                                          curr_sym->ast_val.node_symbol);

                                AstNode *new_expr = node_alloc();
                                add_ast_node_child(curr_var_decl, new_expr);
                                AstNode *sym_name = node_alloc();
                                copy_node(sym_name, curr_sym);
                                if (!set_env(&((*context)->vars), sym_name,
                                             sym_node)) {
                                    print_error(
                                        ERR_COMMON,
                                        "Unable to set environment binding for "
                                        "variable : `%s`",
                                        sym_name->ast_val.node_symbol, 0);
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
                            print_error(ERR_COMMON,
                                        "Unable to set environment binding for "
                                        "variable : `%s`",
                                        sym_name_node->ast_val.node_symbol, 0);
                        }
                        *running_expr = *curr_var_decl;
                    } else
                        print_error(ERR_SYNTAX,
                                    "Could not find variable name in variable "
                                    "declaration",
                                    NULL, 0);
                    return *temp_file_data;
                }
                free_node(res);

                // Lex again to look forward.
                if (check_next_token(":", temp_file_data, &curr_token)) {
                    CHECK_END(
                        **temp_file_data,
                        "End of file during variable re-assignment : `%s`",
                        sym_node->ast_val.node_symbol);

                    AstNode *var_bind =
                        get_env((*context)->vars, sym_node, &status);
                    if (status) {
                        // re-assignment or redefinition (which is an error),
                        // otherwise invalid syntax error.

                        // Lex again to look forward.
                        if (check_next_token("=", temp_file_data,
                                             &curr_token)) {
                            CHECK_END(**temp_file_data,
                                      "End of file during variable "
                                      "re-assignment : `%s`",
                                      sym_node->ast_val.node_symbol);

                            AstNode *new_expr = node_alloc();
                            AstNode *node_reassign = node_alloc();
                            node_reassign->type = TYPE_VAR_REASSIGNMENT;

                            add_ast_node_child(node_reassign, sym_node);
                            add_ast_node_child(node_reassign, new_expr);

                            *running_expr = *node_reassign;
                            running_expr = new_expr;
                            continue;

                        } else {
                            print_error(ERR_COMMON,
                                        "Undefined symbol after : `%s`",
                                        sym_node->ast_val.node_symbol, 0);
                        }
                    } else
                        print_error(ERR_COMMON, "Undefined symbol after : `%s`",
                                    sym_node->ast_val.node_symbol, 0);
                    free_node(var_bind);
                    return *temp_file_data;
                } else {
                    if (check_next_token("(", temp_file_data, &curr_token)) {
                        status = -1;
                        parser_get_func(*context, sym_node, &status);
                        if (status) {
                            running_expr->type = TYPE_FUNCTION_CALL;
                            add_ast_node_child(running_expr, sym_node);
                            AstNode *arg_list = node_alloc();
                            AstNode *curr_arg = node_alloc();
                            add_ast_node_child(arg_list, curr_arg);
                            add_ast_node_child(running_expr, arg_list);
                            running_expr = curr_arg;

                            *context = create_parsing_context(*context);
                            (*context)->op = create_node_symbol("func_call");
                            (*context)->res = running_expr;
                            continue;
                        } else {
                            print_error(ERR_COMMON,
                                        "Function definition not found :"
                                        "`%s`",
                                        sym_node->ast_val.node_symbol, 0);
                        }
                    }
                    // if ((*context)->parent_ctx == NULL)
                    //     print_error(ERR_COMMON,
                    //                 "Undefined symbol :"
                    //                 "`%s`",
                    //                 sym_node->ast_val.node_symbol, 0);
                }
            }
        }

        if (!strncmp_lexed_token(curr_token, ")") &&
            parse_binary_infix_op(temp_file_data, &curr_token, context,
                                  &running_precedence, curr_expr,
                                  &running_expr))
            continue;

        if ((*context)->parent_ctx == NULL)
            break;

        AstNode *op = (*context)->op;
        if (op->type != TYPE_SYM)
            print_error(ERR_COMMON, "Compiler - Context operator not a symbol",
                        NULL, 0);

        if (strcmp(op->ast_val.node_symbol, "def_func") == 0) {
            if (strncmp_lexed_token(curr_token, "}") ||
                check_next_token("}", temp_file_data, &curr_token)) {
                *context = (*context)->parent_ctx;
                break;
            }
        }

        if (strcmp(op->ast_val.node_symbol, "func_call") == 0) {
            if (strncmp_lexed_token(curr_token, ")") ||
                check_next_token(")", temp_file_data, &curr_token)) {
                running_expr = (*context)->res;
                *context = (*context)->parent_ctx;
                if (parse_binary_infix_op(temp_file_data, &curr_token, context,
                                          &running_precedence, curr_expr,
                                          &running_expr))
                    continue;
                break;
            } else if (strncmp_lexed_token(curr_token, ",") ||
                       check_next_token(",", temp_file_data, &curr_token)) {
                (*context)->res->next_child = node_alloc();
                (*context)->res = (*context)->res->next_child;
                running_expr = (*context)->res;
                continue;
            }
        }

        (*context)->res->next_child = node_alloc();
        (*context)->res = (*context)->res->next_child;
        running_expr = (*context)->res;
        continue;
    }
    return *temp_file_data;
}
