#include "../inc/parser.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/helpers.h"
#include "../inc/lexer.h"

int parse_int(LexedToken *token, AstNode *node) {
    if (token == NULL || node == NULL) {
        return 2;
    }
    if (token->token_length == 1 && *(token->token_start) == '0') {
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

ParsingStack *create_parsing_stack(ParsingStack *parent_stack) {
    ParsingStack *new_stack = NULL;
    new_stack = (ParsingStack *)calloc(1, sizeof(ParsingStack));
    CHECK_NULL(new_stack, "Unable to allocate memory for new parsing stack", NULL);
    new_stack->parent_stack = parent_stack;
    new_stack->op = NULL;
    new_stack->res = NULL;
    new_stack->body = NULL;
    return new_stack;
}

ParsingContext *create_parsing_context(ParsingContext *parent_ctx) {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *)calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, "Unable to allocate memory for new parsing context", NULL);
    new_context->child = NULL;
    new_context->next_child = NULL;
    new_context->parent_ctx = parent_ctx;
    new_context->vars = create_env(NULL);
    new_context->env_type = create_env(NULL);
    new_context->funcs = create_env(NULL);
    new_context->binary_ops = create_env(NULL);
    add_parsing_context_child(&parent_ctx, new_context);
    return new_context;
}

ParsingContext *create_default_parsing_context() {
    ParsingContext *new_context = create_parsing_context(NULL);
    AstNode *sym_node = create_node_symbol("int");
    ast_add_type_node(&new_context->env_type, TYPE_INT, sym_node, sizeof(long));
    ast_add_type_node(&new_context->env_type, TYPE_FUNCTION, create_node_symbol("func"),
                      sizeof(long));
    ast_add_binary_ops(&new_context, "=", 3, "int", "int", "int");
    ast_add_binary_ops(&new_context, "<", 3, "int", "int", "int");
    ast_add_binary_ops(&new_context, ">", 3, "int", "int", "int");

    ast_add_binary_ops(&new_context, "+", 5, "int", "int", "int");
    ast_add_binary_ops(&new_context, "-", 5, "int", "int", "int");

    ast_add_binary_ops(&new_context, "*", 10, "int", "int", "int");
    ast_add_binary_ops(&new_context, "/", 10, "int", "int", "int");
    return new_context;
}

void ast_add_type_node(Env **env_type, int node_type, AstNode *sym, long byte_size) {
    if (sym == NULL || byte_size <= 0)
        print_error(ERR_COMMON, "Unable to add a new type to the types environment", NULL, 0);

    AstNode *sz_node = node_alloc();
    sz_node->type = TYPE_INT;
    sz_node->ast_val.val = byte_size;

    AstNode *new_type_node = node_alloc();
    new_type_node->type = node_type;
    new_type_node->child = sz_node;

    if (!set_env(env_type, sym, new_type_node)) {
        print_error(ERR_REDEFINITION, "Unable to redefine type : `%s`", sym->ast_val.node_symbol,
                    0);
    }
}

void ast_add_binary_ops(ParsingContext **context, char *bin_op, int precedence, char *ret_type,
                        char *lhs_type, char *rhs_type) {
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

void add_parsing_context_child(ParsingContext **root, ParsingContext *child_to_add) {
    if (*root == NULL)
        return;

    if ((*root)->child == NULL) {
        (*root)->child = child_to_add;
        return;
    }

    ParsingContext *temp_ctx = (*root)->child;
    while (temp_ctx->next_child != NULL)
        temp_ctx = temp_ctx->next_child;
    temp_ctx->next_child = child_to_add;
}

int parse_binary_infix_op(char **temp_file_data, LexedToken **curr_token, ParsingContext **context,
                          long *running_precedence, AstNode **curr_expr, AstNode **running_expr,
                          ParsingStack *curr_stack) {
    char *tmp_data = *temp_file_data;
    LexedToken *tmp_token = NULL;
    tmp_data = lex_token(&tmp_data, &tmp_token);
    AstNode *node_binary_op = node_symbol_from_token_create(tmp_token);
    int stat = -1;
    ParsingContext *global_ctx = *context;
    while (global_ctx->parent_ctx != NULL)
        global_ctx = global_ctx->parent_ctx;
    AstNode *bin_op_val = get_env(global_ctx->binary_ops, node_binary_op, &stat);
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
            AstNode *temp_curr_expr = *curr_expr;
            if (curr_stack != NULL) {
                *curr_expr = curr_stack->res;
            }

            AstNode *temp_expr_copy = node_alloc();
            copy_node(temp_expr_copy, *curr_expr);
            add_ast_node_child(node_bin_op_body, temp_expr_copy);
            node_bin_op_body->ast_val.node_symbol = strdup(node_binary_op->ast_val.node_symbol);
            node_bin_op_body->next_child = NULL;

            AstNode *rhs_node = node_alloc();
            add_ast_node_child(node_bin_op_body, rhs_node);

            **curr_expr = *node_bin_op_body;
            **running_expr = **curr_expr;
            *running_expr = rhs_node;
            if (curr_stack != NULL)
                *curr_expr = temp_curr_expr;
        } else {
            // Here `running_expr` needs to change, and will store the
            // value of the next integer that needs to be used for the
            // operation based on the operator.
            AstNode *temp_expr_copy = node_alloc();
            copy_node(temp_expr_copy, *running_expr);
            add_ast_node_child(node_bin_op_body, temp_expr_copy);
            node_bin_op_body->ast_val.node_symbol = strdup(node_binary_op->ast_val.node_symbol);
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

void print_parsing_context(ParsingContext *context, int indent) {
    int temp_ident = indent;
    while (temp_ident != 0) {
        putchar(' ');
        temp_ident--;
    }
    printf("TYPES:\n");
    print_env(context->env_type, indent + 2);

    temp_ident = indent;
    while (temp_ident != 0) {
        putchar(' ');
        temp_ident--;
    }
    printf("VARIABLES:\n");
    print_env(context->vars, indent + 2);

    temp_ident = indent;
    while (temp_ident != 0) {
        putchar(' ');
        temp_ident--;
    }
    printf("FUCTIONS:\n");
    print_env(context->funcs, indent + 2);

    if (context->parent_ctx == NULL) {
        temp_ident = indent;
        while (temp_ident != 0) {
            putchar(' ');
            temp_ident--;
        }
        printf("OPERATORS:\n");
        print_env(context->binary_ops, indent + 2);
    }

    ParsingContext *temp_ctx = context->child;
    while (temp_ctx != NULL) {
        print_parsing_context(temp_ctx, indent + 2);
        temp_ctx = temp_ctx->next_child;
    }
}

void lex_and_parse(char *file_dest, ParsingContext **curr_context, AstNode **program) {

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

        temp_file_data = parse_tokens(&temp_file_data, curr_token, &curr_expr, curr_context);
        if (curr_expr->type != TYPE_NULL)
            add_ast_node_child(*program, curr_expr);

        free_node(curr_expr);
    }

    free(file_data);
}

int check_if_delims(LexedToken *token) {
    char *delims = ":=,;~()[]{}<>-+/*&@";
    while (*delims != '\0') {
        if (*token->token_start == *delims)
            return 1;
        delims++;
    }
    return 0;
}

StackOpRetVal stack_operator_continue(ParsingStack **curr_stack, LexedToken **curr_token,
                                      AstNode **running_expr, char **temp_file_data,
                                      ParsingContext **context, long *running_precedence,
                                      AstNode **curr_expr) {
    if (*curr_stack == NULL)
        return STACK_OP_BREAK;

    AstNode *op = (*curr_stack == NULL) ? NULL : (*curr_stack)->op;
    if (op == NULL || op->type != TYPE_SYM)
        print_error(ERR_COMMON, "Compiler - Context operator not a symbol", NULL, 0);

    if (strcmp(op->ast_val.node_symbol, "if-cond") == 0) {
        if (check_next_token("{", temp_file_data, curr_token)) {
            AstNode *if_then_body = node_alloc();
            (*curr_stack)->res->next_child = if_then_body;
            AstNode *if_expr_list = node_alloc();
            add_ast_node_child(if_then_body, if_expr_list);
            if (check_next_token("}", temp_file_data, curr_token)) {
                *curr_stack = (*curr_stack)->parent_stack;
                if (*curr_stack == NULL)
                    return STACK_OP_BREAK;
                else
                    return STACK_OP_CONT_CHECK;
            }
            *running_expr = if_expr_list;
            (*curr_stack)->op = create_node_symbol("if-then-body");
            (*curr_stack)->res = *running_expr;
            (*curr_stack)->body = if_then_body;
            return STACK_OP_CONT_PARSE;
        } else
            print_error(ERR_SYNTAX, "Expected body after the `if` statement", NULL, 0);
    }

    if (strcmp(op->ast_val.node_symbol, "if-then-body") == 0) {
        if (check_next_token("}", temp_file_data, curr_token)) {

            if (check_next_token("else", temp_file_data, curr_token)) {
                if (check_next_token("{", temp_file_data, curr_token)) {

                    print_ast_node((*curr_stack)->res, 0);
                    AstNode *if_else_body = node_alloc();
                    AstNode *if_expr_list = node_alloc();
                    add_ast_node_child(if_else_body, if_expr_list);

                    (*curr_stack)->body->next_child = if_else_body;
                    (*curr_stack)->body = if_else_body;

                    *running_expr = if_expr_list;
                    (*curr_stack)->op = create_node_symbol("if-else-body");
                    (*curr_stack)->res = *running_expr;
                    return STACK_OP_CONT_PARSE;

                } else
                    print_error(ERR_SYNTAX, "Expected body after the `else` statement", NULL, 0);
            }
            *curr_stack = (*curr_stack)->parent_stack;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "if-else-body") == 0) {
        if (check_next_token("}", temp_file_data, curr_token)) {
            *curr_stack = (*curr_stack)->parent_stack;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "def_func") == 0) {
        if (check_next_token("}", temp_file_data, curr_token)) {
            *curr_stack = (*curr_stack)->parent_stack;
            *context = (*context)->parent_ctx;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "lambda") == 0) {
        if (check_next_token("}", temp_file_data, curr_token)) {
            if (strncmp_lexed_token(*curr_token, "]") ||
                check_next_token("]", temp_file_data, curr_token)) {
                *context = (*context)->parent_ctx;
                *curr_stack = (*curr_stack)->parent_stack;
                if (*curr_stack == NULL)
                    return STACK_OP_BREAK;
                else
                    return STACK_OP_CONT_CHECK;
            } else
                print_error(ERR_SYNTAX, "Couldn't find `]` for lambda function", NULL, 0);
        }
    }

    if (strcmp(op->ast_val.node_symbol, "func_call") == 0) {
        if (check_next_token(")", temp_file_data, curr_token)) {
            *running_expr = (*curr_stack)->res;
            *curr_stack = (*curr_stack)->parent_stack;
            if (parse_binary_infix_op(temp_file_data, curr_token, context, running_precedence,
                                      curr_expr, running_expr, *curr_stack))
                return STACK_OP_CONT_PARSE;
            return STACK_OP_CONT_CHECK;
        } else if (strncmp_lexed_token(*curr_token, ",") ||
                   check_next_token(",", temp_file_data, curr_token)) {
            (*curr_stack)->res->next_child = node_alloc();
            (*curr_stack)->res = (*curr_stack)->res->next_child;
            *running_expr = (*curr_stack)->res;
            return STACK_OP_CONT_PARSE;
        }
    }

    if ((*curr_stack)->res->next_child != NULL) {
        (*curr_stack)->res->next_child->next_child = node_alloc();
        (*curr_stack)->res = (*curr_stack)->res->next_child->next_child;
        *running_expr = (*curr_stack)->res;
        return STACK_OP_CONT_PARSE;
    }
    (*curr_stack)->res->next_child = node_alloc();
    (*curr_stack)->res = (*curr_stack)->res->next_child;
    *running_expr = (*curr_stack)->res;
    return STACK_OP_CONT_PARSE;
}

int check_if_type(char *temp_file_data, ParsingContext *context) {
    char *data = temp_file_data;
    LexedToken *token = NULL;
    data = lex_token(&data, &token);
    while (strncmp_lexed_token(token, "@"))
        data = lex_token(&data, &token);
    int stat = -1;
    AstNode *sym = node_symbol_from_token_create(token);
    parser_get_type(context, sym, &stat);
    if (stat)
        return 1;
    return 0;
}

char *parse_tokens(char **temp_file_data, LexedToken *curr_token, AstNode **curr_expr,
                   ParsingContext **context) {

    ParsingStack *curr_stack = NULL;
    AstNode *running_expr = *curr_expr;
    long running_precedence = 0;
    for (;;) {
        *temp_file_data = lex_token(temp_file_data, &curr_token);

        if (curr_token == NULL)
            return *temp_file_data;

        // Check if the current token is an integer.
        if (!parse_int(curr_token, running_expr)) {
            // If the token was not an integer, check if it is
            // variable declaration, assignment, etc.

            if (strncmp_lexed_token(curr_token, "@") && !check_if_type(*temp_file_data, *context)) {
                AstNode *ptr_deref = node_alloc();
                ptr_deref->type = TYPE_DEREFERENCE;
                AstNode *ptr_child = node_alloc();
                add_ast_node_child(ptr_deref, ptr_child);
                *running_expr = *ptr_deref;
                running_expr = ptr_child;
                continue;
            }

            if (strncmp_lexed_token(curr_token, "&")) {
                AstNode *ptr_addr = node_alloc();
                ptr_addr->type = TYPE_ADDROF;
                AstNode *ptr_child = node_alloc();
                add_ast_node_child(ptr_addr, ptr_child);
                *running_expr = *ptr_addr;
                running_expr = ptr_child;
                continue;
            }

            if (strncmp_lexed_token(curr_token, "if")) {
                AstNode *if_node = node_alloc();
                if_node->type = TYPE_IF_CONDITION;

                AstNode *if_expr = node_alloc();
                add_ast_node_child(if_node, if_expr);

                *running_expr = *if_node;
                running_expr = if_expr;

                curr_stack = create_parsing_stack(curr_stack);
                curr_stack->op = create_node_symbol("if-cond");
                curr_stack->res = if_expr;
                continue;
            }

            if (strncmp_lexed_token(curr_token, "[")) {
                AstNode *func_node = node_alloc();
                func_node->type = TYPE_FUNCTION;
                // Get function return type.
                *temp_file_data = lex_token(temp_file_data, &curr_token);
                AstNode *return_type = node_symbol_from_token_create(curr_token);

                // Get parameter list.
                if (!check_next_token("(", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX, "Invalid lambda function definition", NULL, 0);

                AstNode *param_list = node_alloc();
                for (;;) {
                    if (check_next_token(")", temp_file_data, &curr_token))
                        break;

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_type = node_symbol_from_token_create(curr_token);

                    if (!check_next_token(":", temp_file_data, &curr_token))
                        print_error(ERR_SYNTAX, "Couldn't find `:` after parameter type : `%s`",
                                    param_type->ast_val.node_symbol, 0);

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_name = node_symbol_from_token_create(curr_token);

                    AstNode *param = node_alloc();

                    add_ast_node_child(param, param_name);
                    add_ast_node_child(param, param_type);

                    add_ast_node_child(param_list, param);

                    if (!check_next_token(",", temp_file_data, &curr_token)) {
                        if (check_next_token(")", temp_file_data, &curr_token))
                            break;
                        print_error(ERR_SYNTAX, "Could not find end of lambda function definition",
                                    NULL, 0);
                    }
                }
                CHECK_END(**temp_file_data, "End of file - Incomplete lambda function definition",
                          NULL);

                add_ast_node_child(func_node, param_list);
                add_ast_node_child(func_node, return_type);

                if (!check_next_token("{", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX, "Lambda Function definition doesn't have a body", NULL,
                                0);

                *context = create_parsing_context(*context);
                AstNode *params = func_node->child->child;
                while (params != NULL) {
                    if (!set_env(&((*context)->vars), params->child, params->child->next_child)) {
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

                curr_stack = create_parsing_stack(curr_stack);
                curr_stack->op = create_node_symbol("lambda");
                curr_stack->res = func_expr;

                *running_expr = *func_node;
                running_expr = func_expr;
                continue;
            }

            if (strncmp_lexed_token(curr_token, "def_func")) {
                CHECK_END(**temp_file_data, "End of file after : `def_func`", NULL);
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
                    print_error(ERR_SYNTAX, "Invalid function definition for : `%s`",
                                func_name->ast_val.node_symbol, 0);

                AstNode *param_list = node_alloc();
                for (;;) {
                    if (check_next_token(")", temp_file_data, &curr_token))
                        break;

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_type = node_symbol_from_token_create(curr_token);

                    if (!check_next_token(":", temp_file_data, &curr_token))
                        print_error(ERR_SYNTAX, "Couldn't find `:` after parameter type : `%s`",
                                    param_type->ast_val.node_symbol, 0);

                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    AstNode *param_name = node_symbol_from_token_create(curr_token);

                    AstNode *param = node_alloc();

                    add_ast_node_child(param, param_name);
                    add_ast_node_child(param, param_type);

                    add_ast_node_child(param_list, param);

                    if (!check_next_token(",", temp_file_data, &curr_token)) {
                        if (check_next_token(")", temp_file_data, &curr_token))
                            break;
                        print_error(ERR_SYNTAX, "Could not find end of function definition : `%s`",
                                    func_name->ast_val.node_symbol, 0);
                    }
                }
                CHECK_END(**temp_file_data, "End of file - Incomplete function definition : `%s`",
                          func_name->ast_val.node_symbol);

                if (!check_next_token("~", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX, "Return type not specified for function : `%s`",
                                func_name->ast_val.node_symbol, 0);

                *temp_file_data = lex_token(temp_file_data, &curr_token);
                AstNode *return_type = node_symbol_from_token_create(curr_token);

                add_ast_node_child(func_node, param_list);
                add_ast_node_child(func_node, return_type);

                if (!set_env(&((*context)->funcs), func_name, func_node)) {
                    print_error(ERR_COMMON,
                                "Unable to set environment binding for "
                                "function : `%s`",
                                func_name->ast_val.node_symbol, 0);
                }

                if (!check_next_token("{", temp_file_data, &curr_token))
                    print_error(ERR_SYNTAX, "Function definition doesn't have a body : `%s`",
                                func_name->ast_val.node_symbol, 0);

                *context = create_parsing_context(*context);

                AstNode *params = func_node->child->child;
                while (params != NULL) {
                    if (!set_env(&((*context)->vars), params->child, params->child->next_child)) {
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

                curr_stack = create_parsing_stack(curr_stack);
                curr_stack->op = create_node_symbol("def_func");
                curr_stack->res = func_expr;

                *running_expr = *func_node;
                running_expr = func_expr;
                continue;
            } else {
                /**
                 * int a := 340;
                 *
                 * PROGRAM
                 *        `-- VARIABLE_INITIALIZED
                 *            `-- INT (420) -> SYMBOL (a)
                 */
                AstNode *sym_node = node_symbol_from_token_create(curr_token);
                AstNode *type_node = node_alloc();
                AstNode *temp_type_node = type_node;
                while (*sym_node->ast_val.node_symbol == '@') {
                    temp_type_node->type = TYPE_POINTER;
                    AstNode *res_child = node_alloc();
                    temp_type_node->child = res_child;
                    temp_type_node = temp_type_node->child;
                    *temp_file_data = lex_token(temp_file_data, &curr_token);
                    sym_node = node_symbol_from_token_create(curr_token);
                }

                int status = -1;
                AstNode *res = parser_get_type(*context, sym_node, &status);
                if (status) {
                    AstNode *curr_var_decl = node_alloc();
                    curr_var_decl->type = TYPE_VAR_DECLARATION;

                    AstNode *curr_type = node_symbol_from_token_create(curr_token);
                    curr_type->type = res->type;

                    // Lex again to look forward.
                    if (check_next_token(":", temp_file_data, &curr_token)) {
                        CHECK_END(**temp_file_data, "End of file during variable declaration",
                                  NULL);

                        *temp_file_data = lex_token(temp_file_data, &curr_token);

                        AstNode *curr_sym = node_symbol_from_token_create(curr_token);
                        curr_sym->type = TYPE_SYM;

                        get_env((*context)->vars, curr_sym, &status);
                        if (status)
                            print_error(ERR_REDEFINITION, "Redefinition of variable : `%s`",
                                        curr_sym->ast_val.node_symbol, 0);

                        AstNode *sym_name = node_alloc();
                        copy_node(sym_name, curr_sym);
                        add_ast_node_child(curr_var_decl, curr_sym);
                        if (type_node->type == TYPE_POINTER) {
                            *temp_type_node = *sym_node;
                            if (!set_env(&((*context)->vars), sym_name, type_node)) {
                                print_error(ERR_COMMON,
                                            "Unable to set environment binding for "
                                            "variable : `%s`",
                                            sym_name->ast_val.node_symbol, 0);
                            }
                        } else {
                            if (!set_env(&((*context)->vars), sym_name, sym_node)) {
                                print_error(ERR_COMMON,
                                            "Unable to set environment binding for "
                                            "variable : `%s`",
                                            sym_name->ast_val.node_symbol, 0);
                            }
                        }

                        // Lex again to look forward.
                        if (check_next_token(":", temp_file_data, &curr_token)) {
                            CHECK_END(**temp_file_data,
                                      "End of file during variable declaration "
                                      ": `%s`",
                                      curr_sym->ast_val.node_symbol);
                            if (check_next_token("=", temp_file_data, &curr_token)) {
                                CHECK_END(**temp_file_data,
                                          "End of file during variable "
                                          "declaration : `%s`",
                                          curr_sym->ast_val.node_symbol);

                                AstNode *var_reassign = node_alloc();
                                var_reassign->type = TYPE_VAR_REASSIGNMENT;
                                AstNode *new_val = node_alloc();
                                sym_name->type = TYPE_VAR_ACCESS;
                                add_ast_node_child(var_reassign, sym_name);
                                add_ast_node_child(var_reassign, new_val);

                                curr_var_decl->next_child = var_reassign;

                                if (curr_stack != NULL)
                                    *curr_stack->res = *curr_var_decl;
                                *running_expr = *curr_var_decl;
                                running_expr = new_val;
                                continue;
                            }
                        }
                        // If the control flow is here, it means that it is a
                        // variable declaration without intiialization.
                        if (curr_stack != NULL) {
                            *curr_stack->res = *curr_var_decl;
                            StackOpRetVal stack_op_ret = STACK_OP_INVALID;
                            do {
                                stack_op_ret = stack_operator_continue(
                                    &curr_stack, &curr_token, &running_expr, temp_file_data,
                                    context, &running_precedence, curr_expr);
                            } while (stack_op_ret == STACK_OP_CONT_CHECK);
                            if (stack_op_ret == STACK_OP_CONT_PARSE)
                                continue;
                            else if (stack_op_ret == STACK_OP_BREAK)
                                break;
                            else if (stack_op_ret == STACK_OP_INVALID)
                                print_error(
                                    ERR_COMMON,
                                    "Compiler Error - Stack operator not being handled correctly",
                                    NULL, 0);
                        }
                        *running_expr = *curr_var_decl;
                    } else
                        print_error(ERR_SYNTAX,
                                    "Could not find variable name in variable "
                                    "declaration",
                                    NULL, 0);
                }
                free_node(res);

                // If the parsing flow reaches here, it means that we
                // can check a variable access.
                AstNode *node_var_access = NULL;
                if (!check_if_delims(curr_token)) {
                    ParsingContext *temp_ctx = *context;
                    while (temp_ctx != NULL) {
                        status = -1;
                        get_env(temp_ctx->vars, sym_node, &status);
                        if (status)
                            break;
                        temp_ctx = temp_ctx->parent_ctx;
                    }
                    // if (!status)
                    //     print_error(ERR_COMMON,
                    //                 "Undefined symbol :"
                    //                 "`%s`",
                    //                 sym_node->ast_val.node_symbol, 0);
                    if (status) {
                        node_var_access = node_alloc();
                        node_var_access->type = TYPE_VAR_ACCESS;
                        node_var_access->ast_val.node_symbol =
                            strdup(sym_node->ast_val.node_symbol);
                        *running_expr = *node_var_access;
                    }
                }

                // Lex again to look forward.
                if (check_next_token(":", temp_file_data, &curr_token)) {
                    CHECK_END(**temp_file_data, "End of file during variable re-assignment : `%s`",
                              sym_node->ast_val.node_symbol);

                    AstNode *var_bind = parser_get_var(*context, sym_node, &status);
                    if (status) {
                        // re-assignment or redefinition (which is an error),
                        // otherwise invalid syntax error.

                        // Lex again to look forward.
                        if (check_next_token("=", temp_file_data, &curr_token)) {
                            CHECK_END(**temp_file_data,
                                      "End of file during variable "
                                      "re-assignment : `%s`",
                                      sym_node->ast_val.node_symbol);

                            AstNode *new_expr = node_alloc();
                            AstNode *node_reassign = node_alloc();
                            node_reassign->type = TYPE_VAR_REASSIGNMENT;

                            AstNode *temp_expr = node_alloc();
                            if (curr_stack == NULL)
                                copy_node(temp_expr, *curr_expr);
                            else
                                copy_node(temp_expr, curr_stack->res);

                            // VAR_ACCESS needs to be a child of dereference
                            // (which is curr_expr).
                            // TYPE_VAR_REASSIGNMENT needs to be at the top,
                            // which means dereference is a child of this node.
                            add_ast_node_child(node_reassign, temp_expr);

                            // add_ast_node_child(node_reassign, sym_node);
                            add_ast_node_child(node_reassign, new_expr);
                            *running_expr = *node_reassign;
                            running_expr = new_expr;
                            if (curr_stack == NULL)
                                **curr_expr = *node_reassign;
                            else
                                *curr_stack->res = *node_reassign;
                            continue;

                        } else {
                            print_error(ERR_COMMON, "Undefined symbol after : `%s`",
                                        sym_node->ast_val.node_symbol, 0);
                        }
                    } else
                        print_error(ERR_COMMON, "Undefined variable : `%s`",
                                    sym_node->ast_val.node_symbol, 0);
                    free_node(var_bind);
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

                            curr_stack = create_parsing_stack(curr_stack);
                            curr_stack->op = create_node_symbol("func_call");
                            curr_stack->res = running_expr;
                            continue;
                        } else {
                            print_error(ERR_COMMON,
                                        "Function definition not found :"
                                        "`%s`",
                                        sym_node->ast_val.node_symbol, 0);
                        }
                    }
                }
            }
        }

        if (parse_binary_infix_op(temp_file_data, &curr_token, context, &running_precedence,
                                  curr_expr, &running_expr, curr_stack))
            continue;

        if (curr_stack == NULL)
            break;

        StackOpRetVal stack_op_ret = STACK_OP_INVALID;
        do {
            stack_op_ret =
                stack_operator_continue(&curr_stack, &curr_token, &running_expr, temp_file_data,
                                        context, &running_precedence, curr_expr);
        } while (stack_op_ret == STACK_OP_CONT_CHECK);
        if (stack_op_ret == STACK_OP_CONT_PARSE)
            continue;
        else if (stack_op_ret == STACK_OP_BREAK)
            break;
        else if (stack_op_ret == STACK_OP_INVALID)
            print_error(ERR_COMMON, "Compiler Error - Stack operator not being handled correctly",
                        NULL, 0);
    }
    return *temp_file_data;
}
