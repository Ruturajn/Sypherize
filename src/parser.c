#include "../inc/parser.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/lexer.h"
#include "../inc/utils.h"

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
    CHECK_NULL(new_stack, "Unable to allocate memory for new parsing stack",
               NULL);
    new_stack->parent_stack = parent_stack;
    new_stack->op = NULL;
    new_stack->res = NULL;
    new_stack->body = NULL;
    return new_stack;
}

ParsingContext *create_parsing_context(ParsingContext *parent_ctx) {
    ParsingContext *new_context = NULL;
    new_context = (ParsingContext *)calloc(1, sizeof(ParsingContext));
    CHECK_NULL(new_context, "Unable to allocate memory for new parsing context",
               NULL);
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

    ast_add_binary_ops(&new_context, "==", 3, "int", "int", "int");
    ast_add_binary_ops(&new_context, "<", 3, "int", "int", "int");
    ast_add_binary_ops(&new_context, ">", 3, "int", "int", "int");

    ast_add_binary_ops(&new_context, "<<", 4, "int", "int", "int");
    ast_add_binary_ops(&new_context, ">>", 4, "int", "int", "int");

    ast_add_binary_ops(&new_context, "+", 5, "int", "int", "int");
    ast_add_binary_ops(&new_context, "-", 5, "int", "int", "int");

    ast_add_binary_ops(&new_context, "*", 10, "int", "int", "int");
    ast_add_binary_ops(&new_context, "/", 10, "int", "int", "int");
    ast_add_binary_ops(&new_context, "%", 10, "int", "int", "int");

    return new_context;
}

void ast_add_type_node(Env **env_type, int node_type, AstNode *sym,
                       long byte_size) {
    if (sym == NULL || byte_size <= 0)
        print_error(ERR_COMMON,
                    "Unable to add a new type to the types environment");

    AstNode *sz_node = node_alloc();
    sz_node->type = TYPE_INT;
    sz_node->ast_val.val = byte_size;

    AstNode *new_type_node = node_alloc();
    new_type_node->type = node_type;
    new_type_node->child = sz_node;

    if (!set_env(env_type, sym, new_type_node)) {
        print_error(ERR_REDEFINITION, "Unable to redefine type : `%s`",
                    sym->ast_val.node_symbol);
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
                    node_sym_op->ast_val.node_symbol);
    }
}

void add_parsing_context_child(ParsingContext **root,
                               ParsingContext *child_to_add) {
    if (*root == NULL)
        return;

    if ((*root)->child == NULL) {
        if (*root == child_to_add) {
            print_warning(ERR_DEV,
                          "Could not add new child to the parsing context due"
                          "to creation of possible circular linked list");
            return;
        }
        (*root)->child = child_to_add;
        return;
    }

    ParsingContext *temp_ctx = (*root)->child;
    if (temp_ctx == child_to_add) {
        print_warning(ERR_DEV,
                      "Could not add new child to the parsing context due"
                      "to creation of possible circular linked list");
        return;
    }
    while (temp_ctx->next_child != NULL) {
        temp_ctx = temp_ctx->next_child;
        if (temp_ctx == child_to_add) {
            print_warning(ERR_DEV,
                          "Could not add new child to the parsing context due"
                          "to creation of possible circular linked list");
            return;
        }
    }
    temp_ctx->next_child = child_to_add;
}

int parse_binary_infix_op(LexingState **state, ParsingContext **context,
                          long *running_precedence, AstNode **curr_expr,
                          AstNode **running_expr, ParsingStack *curr_stack) {
    LexingState temp_state = **state;
    LexingState *temp_state_ptr = &temp_state;
    lex_token(&temp_state_ptr);
    AstNode *node_binary_op = NULL;

    // While the end of a token isn't a white space character is a delimeter,
    // and isn't a NULL terminator keep extending the token. This is done to
    // catch multi-character operators.
    char *token_end = (temp_state.curr_token->token_start +
                       temp_state.curr_token->token_length);
    while (*token_end != '\0' && strchr(WHITESPACE, *token_end) == NULL &&
           strchr(DELIMS, *token_end) != NULL) {
        token_end += 1;
        temp_state.curr_token->token_length += 1;
        temp_state.file_data += 1;
    }
    node_binary_op = node_symbol_from_token_create(temp_state.curr_token);
    int stat = -1;
    ParsingContext *global_ctx = *context;
    while (global_ctx->parent_ctx != NULL)
        global_ctx = global_ctx->parent_ctx;
    AstNode *bin_op_val =
        get_env(global_ctx->binary_ops, node_binary_op, &stat);
    if (stat) {
        **state = temp_state;
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
            node_bin_op_body->ast_val.node_symbol =
                strdup(node_binary_op->ast_val.node_symbol);
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
    print_env(context->env_type, indent + 4);

    temp_ident = indent;
    while (temp_ident != 0) {
        putchar(' ');
        temp_ident--;
    }
    printf("VARIABLES:\n");
    print_env(context->vars, indent + 4);

    temp_ident = indent;
    while (temp_ident != 0) {
        putchar(' ');
        temp_ident--;
    }
    printf("FUCTIONS:\n");
    print_env(context->funcs, indent + 4);

    if (context->parent_ctx == NULL) {
        temp_ident = indent;
        while (temp_ident != 0) {
            putchar(' ');
            temp_ident--;
        }
        printf("OPERATORS:\n");
        print_env(context->binary_ops, indent + 4);
    }
    putchar('\n');

    ParsingContext *temp_ctx = context->child;
    while (temp_ctx != NULL) {
        print_parsing_context(temp_ctx, indent + 4);
        temp_ctx = temp_ctx->next_child;
    }
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
    AstNode *curr_expr = NULL;

    LexingState *state = calloc(1, sizeof(LexingState));
    CHECK_NULL(state, "Could not allocate memory for LexingState", NULL);
    state->file_data = temp_file_data;
    state->curr_token = curr_token;

    while (*temp_file_data != '\0') {
        curr_expr = node_alloc();

        temp_file_data = parse_tokens(state, &curr_expr, curr_context);
        if (curr_expr->type != TYPE_NULL)
            add_ast_node_child(*program, curr_expr);

        free_node(curr_expr);
    }

    free(file_data);
    free(state->curr_token);
    free(state);
}

int check_if_delims(LexedToken *token) {
    char *delims = DELIMS;
    while (*delims != '\0') {
        if (*token->token_start == *delims)
            return 1;
        delims++;
    }
    return 0;
}

int check_invalid_var_access(ParsingContext *context, AstNode *sym_node) {
    int status = -1;
    ParsingContext *temp_ctx = context;
    while (temp_ctx != NULL) {
        status = -1;
        get_env(temp_ctx->funcs, sym_node, &status);
        if (status)
            return 1;
        get_env(temp_ctx->env_type, sym_node, &status);
        if (status)
            return 1;
        temp_ctx = temp_ctx->parent_ctx;
    }
    if (strcmp(sym_node->ast_val.node_symbol, "else") == 0 ||
        strcmp(sym_node->ast_val.node_symbol, "if") == 0)
        return 1;
    return 0;
}

void parse_func_vars(AstNode **function_type, LexingState **state,
                     ParsingContext *context) {
    AstNode *param_type = node_alloc();
    int status = -1;
    parse_type(&param_type, state, context, &status);
    if (status == 0)
        print_error(ERR_SYNTAX, "Found invalid type in parameter list");

    if (!check_next_token(":", state)) {
        if (!check_next_token(")", state)) {
            print_error(ERR_SYNTAX,
                        "Type annotation must be followed with a `:` in"
                        "function parameter list");
        }
        add_ast_node_child(*function_type, param_type);
        return;
    }

    // Lex past the name, it should be stored in some context atleast!!
    lex_token(state);

    if (check_next_token("(", state)) {
        AstNode *func_type = create_node_symbol("function");
        // Add return type as a child.
        AstNode *func_return_type = node_alloc();
        copy_node(func_return_type, param_type);
        add_ast_node_child(func_type, func_return_type);
        for (;;) {
            if (check_next_token(")", state))
                break;

            lex_token(state);
            parse_func_vars(function_type, state, context);

            if (!check_next_token(",", state)) {
                if (!check_next_token(")", state)) {
                    print_error(ERR_SYNTAX,
                                "Parameter list for function definition"
                                "must be delimited by a `,`");
                }
                break;
            }
        }
        add_ast_node_child(*function_type, func_type);
    } else
        add_ast_node_child(*function_type, param_type);
}

StackOpRetVal
stack_operator_continue(ParsingStack **curr_stack, LexingState **state,
                        AstNode **running_expr, ParsingContext **context,
                        long *running_precedence, AstNode **curr_expr) {
    if (*curr_stack == NULL)
        return STACK_OP_BREAK;

    int valid_op = 0;

    AstNode *op = (*curr_stack == NULL) ? NULL : (*curr_stack)->op;
    if (op == NULL || op->type != TYPE_SYM)
        print_error(ERR_COMMON, "Compiler - Context operator not a symbol");

    if (strcmp(op->ast_val.node_symbol, "if-cond") == 0) {
        valid_op = 1;
        if (check_next_token("{", state)) {
            AstNode *if_then_body = node_alloc();
            (*curr_stack)->res->next_child = if_then_body;
            AstNode *if_expr_list = node_alloc();
            add_ast_node_child(if_then_body, if_expr_list);
            if (check_next_token("}", state)) {
                *context = (*context)->parent_ctx;
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
            print_error(ERR_SYNTAX, "Expected body after the `if` statement");
    }

    if (strcmp(op->ast_val.node_symbol, "if-then-body") == 0) {
        valid_op = 1;
        if (check_next_token("}", state)) {
            // Move up a context, because 'if' is done.
            *context = (*context)->parent_ctx;

            if (check_next_token("else", state)) {
                if (check_next_token("{", state)) {
                    AstNode *if_else_body = node_alloc();
                    AstNode *if_expr_list = node_alloc();
                    add_ast_node_child(if_else_body, if_expr_list);

                    // Create a new context for 'else'.
                    *context = create_parsing_context(*context);

                    (*curr_stack)->body->next_child = if_else_body;
                    (*curr_stack)->body = if_else_body;

                    *running_expr = if_expr_list;
                    (*curr_stack)->op = create_node_symbol("if-else-body");
                    (*curr_stack)->res = *running_expr;
                    return STACK_OP_CONT_PARSE;

                } else
                    print_error(ERR_SYNTAX,
                                "Expected body after the `else` statement");
            }
            *curr_stack = (*curr_stack)->parent_stack;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "if-else-body") == 0) {
        valid_op = 1;
        if (check_next_token("}", state)) {
            *context = (*context)->parent_ctx;
            *curr_stack = (*curr_stack)->parent_stack;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "lambda_body") == 0) {
        valid_op = 1;
        if (check_next_token("}", state)) {
            *curr_stack = (*curr_stack)->parent_stack;
            *context = (*context)->parent_ctx;
            if (*curr_stack == NULL)
                return STACK_OP_BREAK;
            else
                return STACK_OP_CONT_CHECK;
        }
    }

    if (strcmp(op->ast_val.node_symbol, "lambda_params") == 0) {
        valid_op = 1;
        if ((*running_expr)->type != TYPE_VAR_DECLARATION)
            print_error(ERR_SYNTAX,
                        "Found invalid declaration of function parameters");
        int status = -1;
        AstNode *var_type =
            get_env((*context)->vars, (*running_expr)->child, &status);
        if (status == 0)
            print_error(
                ERR_COMMON,
                "Could not find type information for function parameter");

        add_ast_node_child(*running_expr, var_type);
        if (check_next_token(")", state)) {

            // Parse function body.
            if (!check_next_token("{", state))
                print_error(ERR_SYNTAX,
                            "Couldn't find body for function definition");

            if (check_next_token("}", state)) {
                *context = (*context)->parent_ctx;
                *curr_stack = (*curr_stack)->parent_stack;
                if (*curr_stack == NULL)
                    return STACK_OP_BREAK;
                else
                    return STACK_OP_CONT_CHECK;
            }

            (*curr_stack)->op = create_node_symbol("lambda_body");
            AstNode *func_body = node_alloc();
            AstNode *func_expr = node_alloc();
            add_ast_node_child(func_body, func_expr);
            (*curr_stack)->body->next_child = func_body;
            (*curr_stack)->body = (*curr_stack)->body->next_child;
            (*curr_stack)->res = func_expr;
            *running_expr = func_expr;
            return STACK_OP_CONT_PARSE;
        } else {
            // Parse function params
            if (!check_next_token(",", state))
                print_error(ERR_SYNTAX, "Could not find end of parameter list "
                                        "in function definition");
        }
    }

    if (strcmp(op->ast_val.node_symbol, "func_call") == 0) {
        valid_op = 1;
        if (check_next_token(")", state)) {
            *running_expr = (*curr_stack)->res;
            *curr_stack = (*curr_stack)->parent_stack;
            if (parse_binary_infix_op(state, context, running_precedence,
                                      curr_expr, running_expr, *curr_stack))
                return STACK_OP_CONT_PARSE;
            return STACK_OP_CONT_CHECK;
        } else if (strncmp_lexed_token((*state)->curr_token, ",") ||
                   check_next_token(",", state)) {
            (*curr_stack)->res->next_child = node_alloc();
            (*curr_stack)->res = (*curr_stack)->res->next_child;
            *running_expr = (*curr_stack)->res;
            return STACK_OP_CONT_PARSE;
        }
    }

    if (valid_op == 0)
        return STACK_OP_INVALID;

    (*curr_stack)->res->next_child = node_alloc();
    (*curr_stack)->res = (*curr_stack)->res->next_child;
    *running_expr = (*curr_stack)->res;
    return STACK_OP_CONT_PARSE;
}

AstNode *parse_type(AstNode **type_node, LexingState **state,
                    ParsingContext *context, int *status) {
    unsigned int pointer_indirect = 0;
    while (*(((*state)->curr_token)->token_start) == '@') {
        pointer_indirect += 1;
        lex_token(state);
    }

    AstNode *sym_node = node_symbol_from_token_create((*state)->curr_token);

    AstNode *res = parser_get_type(context, sym_node, status);
    if (*status == 0)
        return NULL;

    if (check_next_token("[", state)) {
        lex_token(state);

        AstNode *arr_size = node_alloc();
        if (!parse_int((*state)->curr_token, arr_size))
            print_error(ERR_SYNTAX,
                        "Unable to parse array size, expected valid integer");

        if (!check_next_token("]", state))
            print_error(ERR_SYNTAX,
                        "Expected `]` after declaration of array size");

        AstNode *arr_type = create_node_symbol("array");
        AstNode *prev_type = node_alloc();
        copy_node(prev_type, sym_node);
        add_ast_node_child(arr_type, arr_size);
        add_ast_node_child(arr_type, prev_type);
        copy_node(sym_node, arr_type);
        free(prev_type);
    }

    copy_node(*type_node, sym_node);
    (*type_node)->pointer_level = pointer_indirect;

    // char *temp_file_data_ptr = *temp_file_data;
    // LexedToken *temp_token = NULL;
    // if (check_next_token(":", &temp_file_data_ptr, &temp_token)) {
    //     if (check_next_token("(", &temp_file_data_ptr, &temp_token)) {
    //         // If we are here, it means we are parsing a function type.
    //         *temp_file_data = temp_file_data_ptr;
    //         **curr_token = *temp_token;
    //         AstNode *function_type = create_node_symbol("function");
    //         // Add return type as a child.
    //         AstNode *func_ret_type = node_alloc();
    //         copy_node(func_ret_type, *type_node);
    //         add_ast_node_child(function_type, func_ret_type);

    //         for (;;) {
    //             if (check_next_token(")", temp_file_data, curr_token))
    //                 break;
    //             *temp_file_data = lex_token(temp_file_data, curr_token);
    //             AstNode *param_type = node_alloc();
    //             parse_type(&param_type, curr_token, temp_file_data, context,
    //                        status);
    //             add_ast_node_child(function_type, param_type);

    //             if (!check_next_token(",", temp_file_data, curr_token)) {
    //                 if (!check_next_token(")", temp_file_data, curr_token))
    //                     print_error(ERR_SYNTAX,
    //                                 "Expected `,` between function
    //                                 parameters");
    //                 else
    //                     break;
    //             }
    //         }
    //         copy_node(*type_node, function_type);
    //     }
    // }
    return res;
}

int check_if_type(char *temp_file_data, ParsingContext *context) {
    LexingState temp_state;
    temp_state.file_data = temp_file_data;
    temp_state.curr_token = NULL;
    LexingState *temp_state_ptr = &temp_state;
    lex_token(&temp_state_ptr);
    while (strncmp_lexed_token(temp_state.curr_token, "@"))
        lex_token(&temp_state_ptr);
    int stat = -1;
    AstNode *sym = node_symbol_from_token_create(temp_state.curr_token);
    parser_get_type(context, sym, &stat);
    if (stat)
        return 1;
    return 0;
}

char *parse_tokens(LexingState *state, AstNode **curr_expr,
                   ParsingContext **context) {

    ParsingStack *curr_stack = NULL;
    AstNode *running_expr = *curr_expr;
    long running_precedence = 0;
    for (;;) {
        lex_token(&state);

        if (*state->curr_token->token_start == '\0')
            return state->file_data;

        // Check if the current token is an integer.
        if (!parse_int(state->curr_token, running_expr)) {
            // If the token was not an integer, check if it is
            // variable declaration, assignment, etc.

            if (strncmp_lexed_token(state->curr_token, "@") &&
                !check_if_type(state->file_data, *context)) {
                AstNode *ptr_deref = node_alloc();
                ptr_deref->type = TYPE_DEREFERENCE;
                AstNode *ptr_child = node_alloc();
                add_ast_node_child(ptr_deref, ptr_child);
                *running_expr = *ptr_deref;
                running_expr = ptr_child;
                continue;
            }

            if (strncmp_lexed_token(state->curr_token, "&")) {
                AstNode *ptr_addr = node_alloc();
                ptr_addr->type = TYPE_ADDROF;
                AstNode *ptr_child = node_alloc();
                add_ast_node_child(ptr_addr, ptr_child);
                *running_expr = *ptr_addr;
                running_expr = ptr_child;
                continue;
            }

            if (strncmp_lexed_token(state->curr_token, "if")) {
                AstNode *if_node = node_alloc();
                if_node->type = TYPE_IF_CONDITION;

                AstNode *if_expr = node_alloc();
                add_ast_node_child(if_node, if_expr);

                *running_expr = *if_node;
                running_expr = if_expr;

                *context = create_parsing_context(*context);

                curr_stack = create_parsing_stack(curr_stack);
                curr_stack->op = create_node_symbol("if-cond");
                curr_stack->res = if_expr;
                continue;
            }

            /**
             * int a := 340;
             *
             * PROGRAM
             *        `-- VARIABLE_INITIALIZED
             *            `-- INT (420) -> SYMBOL (a)
             */
            AstNode *type_node = node_alloc();

            int status = -1;
            char is_external = 0;

            if (strncmp_lexed_token(state->curr_token, "ext")) {
                is_external = 1;
                lex_token(&state);
            }

            AstNode *res = parse_type(&type_node, &state, *context, &status);
            if (status) {
                AstNode *curr_var_decl = node_alloc();
                curr_var_decl->type = TYPE_VAR_DECLARATION;

                AstNode *curr_type =
                    node_symbol_from_token_create(state->curr_token);
                curr_type->type = res->type;

                // Lex again to look forward.
                if (check_next_token(":", &state)) {
                    CHECK_END(*state->file_data,
                              "End of file during variable declaration", NULL);

                    // Handle lambda functions.
                    if (check_next_token("(", &state)) {
                        if (is_external)
                            print_error(ERR_SYNTAX,
                                        "`ext` keyword cannot be used with "
                                        "lambda functions");
                        AstNode *lambda_func_node = node_alloc();
                        lambda_func_node->type = TYPE_FUNCTION;
                        AstNode *arg_list = node_alloc();
                        add_ast_node_child(lambda_func_node, type_node);
                        add_ast_node_child(lambda_func_node, arg_list);

                        if (check_next_token(")", &state)) {

                            // Parse function body.
                            if (!check_next_token("{", &state))
                                print_error(ERR_SYNTAX,
                                            "Couldn't find body for function "
                                            "definition");

                            if (check_next_token("}", &state)) {
                                *running_expr = *lambda_func_node;
                                break;
                            }

                            *context = create_parsing_context(*context);
                            curr_stack = create_parsing_stack(curr_stack);
                            curr_stack->op = create_node_symbol("lambda_body");
                            curr_stack->body = type_node;
                            curr_stack->body->next_child = arg_list;
                            curr_stack->body = curr_stack->body->next_child;

                            AstNode *func_body = node_alloc();
                            AstNode *func_expr = node_alloc();
                            add_ast_node_child(func_body, func_expr);
                            add_ast_node_child(lambda_func_node, func_body);

                            curr_stack->body->next_child = func_body;
                            curr_stack->body = curr_stack->body->next_child;
                            curr_stack->res = func_expr;
                            *running_expr = *lambda_func_node;
                            running_expr = func_expr;
                            continue;
                        }

                        AstNode *curr_arg = node_alloc();
                        add_ast_node_child(arg_list, curr_arg);
                        *running_expr = *lambda_func_node;
                        running_expr = curr_arg;
                        *context = create_parsing_context(*context);
                        curr_stack = create_parsing_stack(curr_stack);
                        curr_stack->op = create_node_symbol("lambda_params");
                        curr_stack->body = type_node;
                        curr_stack->body->next_child = arg_list;
                        curr_stack->body = curr_stack->body->next_child;
                        curr_stack->res = running_expr;
                        continue;
                    }
                    lex_token(&state);
                    AstNode *curr_sym =
                        node_symbol_from_token_create(state->curr_token);
                    curr_sym->type = TYPE_SYM;

                    get_env((*context)->vars, curr_sym, &status);
                    if (status)
                        print_error(ERR_REDEFINITION,
                                    "Redefinition of variable : `%s`",
                                    curr_sym->ast_val.node_symbol);

                    AstNode *sym_name = node_alloc();
                    copy_node(sym_name, curr_sym);
                    add_ast_node_child(curr_var_decl, curr_sym);

                    // Handling functions as variables.
                    if (check_next_token("(", &state)) {
                        AstNode *function_type = NULL;
                        if (is_external)
                            function_type = create_node_symbol("ext function");
                        else
                            function_type = create_node_symbol("function");
                        // Add return type as a child.
                        add_ast_node_child(function_type, type_node);

                        for (;;) {
                            if (check_next_token(")", &state))
                                break;

                            lex_token(&state);
                            parse_func_vars(&function_type, &state, *context);

                            if (!check_next_token(",", &state)) {
                                if (!check_next_token(")", &state)) {
                                    print_error(
                                        ERR_SYNTAX,
                                        "Parameter list for function definition"
                                        "must be delimited by a `,`");
                                }
                                break;
                            }
                        }

                        if (!set_env(&((*context)->vars), sym_name,
                                     function_type)) {
                            print_error(ERR_COMMON,
                                        "Unable to set environment binding for "
                                        "variable : `%s`",
                                        sym_name->ast_val.node_symbol);
                        }
                    } else {
                        if (!set_env(&((*context)->vars), sym_name,
                                     type_node)) {
                            print_error(ERR_COMMON,
                                        "Unable to set environment binding for "
                                        "variable : `%s`",
                                        sym_name->ast_val.node_symbol);
                        }
                    }

                    // Lex again to look forward.
                    if (check_next_token(":", &state)) {
                        CHECK_END(*state->file_data,
                                  "End of file during variable declaration "
                                  ": `%s`",
                                  curr_sym->ast_val.node_symbol);
                        if (check_next_token("=", &state)) {
                            CHECK_END(*state->file_data,
                                      "End of file during variable "
                                      "declaration : `%s`",
                                      curr_sym->ast_val.node_symbol);

                            AstNode *var_reassign = node_alloc();
                            var_reassign->type = TYPE_VAR_REASSIGNMENT;
                            AstNode *new_val = node_alloc();
                            AstNode *sym_name_copy = node_alloc();
                            copy_node(sym_name_copy, sym_name);
                            sym_name_copy->type = TYPE_VAR_ACCESS;
                            add_ast_node_child(var_reassign, sym_name_copy);
                            add_ast_node_child(var_reassign, new_val);

                            curr_var_decl->next_child = var_reassign;

                            *running_expr = *curr_var_decl;
                            running_expr = new_val;
                            if (curr_stack != NULL) {
                                *curr_stack->res = *curr_var_decl;
                                curr_stack->res = var_reassign;
                            }
                            continue;
                        }
                        print_error(ERR_SYNTAX,
                                    "Expected `=` after `:` in variable "
                                    "declaration for : `%s`",
                                    sym_name->ast_val.node_symbol);
                    }
                    if (check_next_token("=", &state))
                        print_error(ERR_SYNTAX,
                                    "Expected `:` before `=` in variable "
                                    "declaration for : `%s`",
                                    sym_name->ast_val.node_symbol);

                    // If the control flow is here, it means that it is a
                    // variable declaration without intiialization.
                    if (curr_stack != NULL) {
                        *curr_stack->res = *curr_var_decl;
                        StackOpRetVal stack_op_ret = STACK_OP_INVALID;
                        do {
                            stack_op_ret = stack_operator_continue(
                                &curr_stack, &state, &running_expr, context,
                                &running_precedence, curr_expr);
                        } while (stack_op_ret == STACK_OP_CONT_CHECK);
                        if (stack_op_ret == STACK_OP_CONT_PARSE)
                            continue;
                        else if (stack_op_ret == STACK_OP_BREAK)
                            break;
                        else if (stack_op_ret == STACK_OP_INVALID)
                            print_error(ERR_COMMON,
                                        "Compiler Error - Stack operator not "
                                        "being handled correctly");
                    }
                    *running_expr = *curr_var_decl;
                    break;
                } else
                    print_error(ERR_SYNTAX,
                                "Expected `:` after type declaration");
            }
            free_node(res);

            if (is_external)
                print_error(ERR_SYNTAX,
                            "`ext` keyword can only be used for functions");

            AstNode *sym_node =
                node_symbol_from_token_create(state->curr_token);
            // If the parsing flow reaches here, it means that we
            // can check a variable access.
            AstNode *node_var_access = NULL;
            if (!check_if_delims(state->curr_token) &&
                !check_invalid_var_access(*context, sym_node)) {
                ParsingContext *temp_ctx = *context;
                while (temp_ctx != NULL) {
                    status = -1;
                    get_env(temp_ctx->vars, sym_node, &status);
                    if (status)
                        break;
                    temp_ctx = temp_ctx->parent_ctx;
                }
                if (status == 0)
                    print_error(ERR_COMMON,
                                "Undefined symbol :"
                                "`%s`",
                                sym_node->ast_val.node_symbol);

                node_var_access = node_alloc();
                node_var_access->type = TYPE_VAR_ACCESS;
                node_var_access->ast_val.node_symbol =
                    strdup(sym_node->ast_val.node_symbol);
                *running_expr = *node_var_access;

                if (check_next_token("[", &state)) {
                    lex_token(&state);
                    AstNode *array_index = node_alloc();

                    if (!parse_int(state->curr_token, array_index))
                        print_error(ERR_SYNTAX,
                                    "Expected valid integer index for "
                                    "accessing array type : `%s`",
                                    node_var_access->ast_val.node_symbol);

                    if (!check_next_token("]", &state))
                        print_error(
                            ERR_SYNTAX,
                            "Expected `]` after indexing array type : `%s`",
                            node_var_access->ast_val.node_symbol);

                    AstNode *arr_access = node_alloc();
                    arr_access->type = TYPE_ARR_INDEX;
                    arr_access->ast_val.val = array_index->ast_val.val;
                    add_ast_node_child(arr_access, node_var_access);
                    *running_expr = *arr_access;
                }
            }

            // Lex again to look forward.
            if (check_next_token(":", &state)) {
                CHECK_END(*state->file_data,
                          "End of file during variable re-assignment : `%s`",
                          sym_node->ast_val.node_symbol);

                AstNode *var_bind = parser_get_var(*context, sym_node, &status);
                if (status) {
                    // re-assignment or redefinition (which is an error),
                    // otherwise invalid syntax error.

                    // Lex again to look forward.
                    if (check_next_token("=", &state)) {
                        CHECK_END(*state->file_data,
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
                                    sym_node->ast_val.node_symbol);
                    }
                } else
                    print_error(ERR_COMMON, "Undefined variable : `%s`",
                                sym_node->ast_val.node_symbol);
                free_node(var_bind);
            } else {
                if (curr_stack == NULL && check_next_token("=", &state))
                    print_error(ERR_SYNTAX,
                                "Expected `:` before `=` in variable "
                                "re-assignment of : `%s`",
                                node_var_access->ast_val.node_symbol);
                if (check_next_token("(", &state)) {
                    status = -1;
                    parser_get_var(*context, sym_node, &status);
                    if (status) {
                        running_expr->type = TYPE_FUNCTION_CALL;
                        AstNode *var_func_name = node_alloc();
                        copy_node(var_func_name, sym_node);
                        var_func_name->type = TYPE_VAR_ACCESS;
                        add_ast_node_child(running_expr, var_func_name);
                        AstNode *arg_list = node_alloc();

                        if (check_next_token(")", &state)) {
                            add_ast_node_child(running_expr, arg_list);
                            if (parse_binary_infix_op(
                                    &state, context, &running_precedence,
                                    curr_expr, &running_expr, curr_stack))
                                continue;
                            StackOpRetVal stack_op_ret = STACK_OP_INVALID;
                            do {
                                stack_op_ret = stack_operator_continue(
                                    &curr_stack, &state, &running_expr, context,
                                    &running_precedence, curr_expr);
                            } while (stack_op_ret == STACK_OP_CONT_CHECK);
                            if (stack_op_ret == STACK_OP_CONT_PARSE)
                                continue;
                            else if (stack_op_ret == STACK_OP_BREAK)
                                break;
                            else if (stack_op_ret == STACK_OP_INVALID)
                                print_error(ERR_COMMON,
                                            "Compiler Error - Stack operator "
                                            "not being handled "
                                            "correctly");
                        }

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
                                    sym_node->ast_val.node_symbol);
                    }
                }
            }
        }

        if (parse_binary_infix_op(&state, context, &running_precedence,
                                  curr_expr, &running_expr, curr_stack))
            continue;

        if (curr_stack == NULL)
            break;

        StackOpRetVal stack_op_ret = STACK_OP_INVALID;
        do {
            stack_op_ret = stack_operator_continue(
                &curr_stack, &state, &running_expr, context,
                &running_precedence, curr_expr);
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
    return state->file_data;
}
