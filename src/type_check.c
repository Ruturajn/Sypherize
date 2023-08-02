#include "../inc/type_check.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/helpers.h"
#include "../inc/parser.h"

int cmp_type(AstNode *node1, AstNode *node2) {
    if (node1->type != node2->type)
        return 0;
    AstNode *temp_node1 = node1->child;
    AstNode *temp_node2 = node2->child;
    while (temp_node1 != NULL && temp_node2 != NULL) {
        if (cmp_type(temp_node1, temp_node2) == 0)
            return 0;
        temp_node1 = temp_node1->next_child;
        temp_node2 = temp_node2->next_child;
    }
    if (temp_node1 == temp_node2)
        return 1;
    return 1;
}

AstNode *get_return_type(ParsingContext *context, ParsingContext **context_to_enter,
                         AstNode *expr) {
    AstNode *ret_type = node_alloc();
    int stat = -1;
    switch (expr->type) {
    case TYPE_ADDROF:;
        type_check_expr(context, context_to_enter, expr);
        AstNode *temp = get_return_type(context, context_to_enter, expr->child);
        ret_type->type = TYPE_POINTER;
        add_ast_node_child(ret_type, temp);
        break;
    case TYPE_DEREFERENCE:;
        type_check_expr(context, context_to_enter, expr);
        type_check_expr(context, context_to_enter, expr->child);
        ret_type = get_return_type(context, context_to_enter, expr->child);
        memcpy(ret_type, ret_type->child, sizeof(AstNode));
        break;
    case TYPE_VAR_ACCESS:;
        ParsingContext *temp_ctx = context;
        AstNode *sym_type = NULL;
        if (temp_ctx->parent_ctx == NULL) {
            sym_type = get_env_from_sym(temp_ctx->vars, expr->ast_val.node_symbol, &stat);
            if (stat == 0)
                print_error(ERR_COMMON, "Couldn't find information for variable : `%s`",
                            expr->ast_val.node_symbol, 0);
        } else {
            while (temp_ctx->parent_ctx != NULL) {
                sym_type = get_env_from_sym(temp_ctx->vars, expr->ast_val.node_symbol, &stat);
                if (stat)
                    break;
                temp_ctx = temp_ctx->parent_ctx;
            }
            if (stat == 0)
                print_error(ERR_COMMON, "Couldn't find information for variable : `%s`",
                            expr->ast_val.node_symbol, 0);
        }
        // sym_type POINTER
        //          `-- SYM : int

        // The above node needs to be converted to:
        // POINTER
        //       `-- INT : 0
        copy_node(ret_type, sym_type);
        AstNode *temp_node = ret_type;
        while (temp_node->child != NULL)
            temp_node = temp_node->child;

        AstNode *type_node = parser_get_type(temp_ctx, temp_node, &stat);
        if (stat == 0)
            print_error(ERR_COMMON, "Couldn't find information for `type` of variable : `%s`",
                        expr->ast_val.node_symbol, 0);
        *temp_node = *type_node;
        break;
    case TYPE_BINARY_OPERATOR:
        type_check_expr(context, context_to_enter, expr);
        temp_ctx = context;
        while (temp_ctx->parent_ctx != NULL)
            temp_ctx = temp_ctx->parent_ctx;
        AstNode *op_sym = create_node_symbol(expr->ast_val.node_symbol);
        AstNode *op_data = get_env(temp_ctx->binary_ops, op_sym, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->next_child->ast_val.node_symbol, 0);
        AstNode *op_decl_ret_type = parser_get_type(temp_ctx, op_data->child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->ast_val.node_symbol, 0);
        ret_type = op_decl_ret_type;
        free_node(op_sym);
        free_node(op_data);
        break;
    case TYPE_FUNCTION_CALL:;
        AstNode *func_body = parser_get_func(context, expr->child, &stat);
        if (!stat)
            print_error(ERR_COMMON,
                        "Function definition not found :"
                        "`%s`",
                        expr->child->ast_val.node_symbol, 0);
        AstNode *func_ret = parser_get_type(context, func_body->child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        func_body->child->next_child->ast_val.node_symbol, 0);
        ret_type = func_ret;
        /* free_node(func_ret); */
        free_node(func_body);
        break;
    default:
        ret_type = expr;
        break;
    }
    return ret_type;
}

void type_check_expr(ParsingContext *context, ParsingContext **context_to_enter, AstNode *expr) {
    AstNode *temp_expr = expr;
    int stat = -1;

    // AstNode *expr_child = expr->child;
    // while (expr_child != NULL) {
    //     type_check_expr(context, expr_child);
    //     expr_child = expr_child->next_child;
    // }

    switch (temp_expr->type) {
    case TYPE_ADDROF:;
        if (temp_expr->child->type != TYPE_VAR_ACCESS)
            print_error(ERR_SYNTAX, "Expected valid variable access for AddressOf operator", NULL,
                        0);
        break;
    case TYPE_DEREFERENCE:;
        AstNode *deref_type = get_return_type(context, context_to_enter, expr->child);
        if (deref_type->type != TYPE_POINTER)
            print_error(ERR_TYPE, "Only pointer types can be dereferenced", NULL, 0);
        break;
    case TYPE_FUNCTION:;
        ParsingContext *to_enter = (*context_to_enter)->child;
        AstNode *function_body = temp_expr->child->next_child->next_child->child;
        AstNode *last_expr = function_body;
        while (function_body != NULL) {
            type_check_expr(*context_to_enter, &to_enter, function_body);
            last_expr = function_body;
            function_body = function_body->next_child;
        }
        AstNode *last_expr_type = get_return_type(*context_to_enter, &to_enter, last_expr);
        AstNode *func_ret_type = parser_get_type(context, temp_expr->child->next_child, &stat);
        if (stat == 0)
            print_error(ERR_COMMON, "Couldn't find information for return type of the function",
                        NULL, 0);
        if (cmp_type(last_expr_type, func_ret_type) == 0)
            print_error(ERR_TYPE,
                        "Found Mismatched type for function return type and last expression", NULL,
                        0);

        (*context_to_enter) = (*context_to_enter)->next_child;
        break;
    case TYPE_VAR_REASSIGNMENT:;
        // Get the return type of the left hand side of a variable declaration.
        AstNode *lhs_ret_type = get_return_type(context, context_to_enter, temp_expr->child);
        // Get the return type of the left hand side of a variable declaration.
        AstNode *rhs_ret_type =
            get_return_type(context, context_to_enter, temp_expr->child->next_child);
        if (cmp_type(lhs_ret_type, rhs_ret_type) == 0) {
            printf("Expression:\n");
            print_ast_node(temp_expr, 0);
            printf("LHS TYPE:\n");
            print_ast_node(lhs_ret_type, 0);
            printf("RHS TYPE:\n");
            print_ast_node(rhs_ret_type, 0);
            print_error(ERR_TYPE, "Mismatched types for variable re-assignment", NULL, 0);
        }
        break;
    case TYPE_BINARY_OPERATOR:;
        ParsingContext *temp_ctx = context;
        while (temp_ctx->parent_ctx != NULL)
            temp_ctx = temp_ctx->parent_ctx;
        AstNode *op_sym = create_node_symbol(temp_expr->ast_val.node_symbol);
        AstNode *op_data = get_env(temp_ctx->binary_ops, op_sym, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        AstNode *op_used_lhs_type = get_return_type(context, context_to_enter, expr->child);
        AstNode *op_decl_lhs_type =
            parser_get_type(temp_ctx, op_data->child->next_child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->next_child->ast_val.node_symbol, 0);
        if (cmp_type(op_used_lhs_type, op_decl_lhs_type) == 0)
            print_error(ERR_COMMON, "Found Mismatched LHS type for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        AstNode *op_used_rhs_type =
            get_return_type(context, context_to_enter, expr->child->next_child);
        AstNode *op_decl_rhs_type =
            parser_get_type(temp_ctx, op_data->child->next_child->next_child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->next_child->next_child->ast_val.node_symbol, 0);
        if (cmp_type(op_used_rhs_type, op_decl_rhs_type) == 0)
            print_error(ERR_COMMON, "Found Mismatched RHS type for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        free_node(op_sym);
        free_node(op_data);
        free_node(op_decl_lhs_type);
        free_node(op_decl_rhs_type);
        break;
    case TYPE_FUNCTION_CALL:;
        AstNode *func_body = parser_get_func(context, temp_expr->child, &stat);
        if (!stat) {
            print_error(ERR_COMMON,
                        "Function definition not found :"
                        "`%s`",
                        temp_expr->child->ast_val.node_symbol, 0);
        }
        AstNode *func_param_list = func_body->child->child;
        AstNode *func_call_params = temp_expr->child->next_child->child;
        AstNode *param_list_type = NULL;
        AstNode *param_call_type = NULL;
        AstNode *temp_param_list_type = NULL;
        AstNode *func_param_it = NULL;
        while (func_call_params != NULL && func_param_list != NULL) {
            param_call_type = get_return_type(context, context_to_enter, func_call_params);

            AstNode *complete_param_list_type = node_alloc();
            copy_node(complete_param_list_type, func_param_list->child->next_child);
            func_param_it = func_param_list->child->next_child;
            temp_param_list_type = complete_param_list_type;

            while (func_param_it != NULL && func_param_it->type != TYPE_SYM) {
                temp_param_list_type->child = node_alloc();
                temp_param_list_type->child->type = func_param_it->type;
                temp_param_list_type = temp_param_list_type->child;
                func_param_it = func_param_it->child;
            }
            param_list_type = parser_get_type(context, func_param_it, &stat);
            *temp_param_list_type = *param_list_type;

            if (stat == 0)
                print_error(ERR_TYPE, "Unable to find type information for : `%s`",
                            func_param_list->child->next_child->ast_val.node_symbol, 0);
            if (param_call_type->type == TYPE_NULL)
                break;
            if (cmp_type(param_call_type, complete_param_list_type) == 0) {
                print_error(ERR_SYNTAX, "Mismatched argument type for function call : `%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            }
            func_param_list = func_param_list->next_child;
            func_call_params = func_call_params->next_child;
            free_node(complete_param_list_type);
        }
        if (func_param_list != NULL) {
            print_error(ERR_ARGS,
                        "Too few arguments for function : "
                        "`%s`",
                        temp_expr->child->ast_val.node_symbol, 0);
        }
        if (func_call_params != NULL) {
            print_error(ERR_ARGS,
                        "Too many arguments passed to function : "
                        "`%s`",
                        temp_expr->child->ast_val.node_symbol, 0);
        }
        free_node(param_list_type);
        free_node(func_body);
        break;
    default:
        break;
    }
}

void type_check_prog(ParsingContext *context, AstNode *prog) {
    AstNode *temp_expr = prog->child;

    ParsingContext *context_to_enter = context->child;

    while (temp_expr != NULL) {
        type_check_expr(context, &context_to_enter, temp_expr);
        temp_expr = temp_expr->next_child;
    }
}
