#include "../inc/type_check.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/helpers.h"
#include "../inc/parser.h"

int get_return_type(ParsingContext *context, AstNode *expr) {
    int ret_type = -1;
    int stat = -1;
    switch (expr->type) {
    case TYPE_VAR_ACCESS:;
        ParsingContext *temp_ctx = context;
        while (temp_ctx->parent_ctx != NULL)
            temp_ctx = temp_ctx->parent_ctx;
        AstNode *sym_type = get_env_from_sym(temp_ctx->vars, expr->ast_val.node_symbol, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for variable : `%s`",
                        expr->ast_val.node_symbol, 0);
        AstNode *type_node = parser_get_type(temp_ctx, sym_type, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for `type` of variable : `%s`",
                        expr->ast_val.node_symbol, 0);
        ret_type = type_node->type;
        break;
    case TYPE_BINARY_OPERATOR:
        type_check_expr(context, expr);
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
        ret_type = op_decl_ret_type->type;
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
        ret_type = func_ret->type;
        free_node(func_ret);
        free_node(func_body);
        break;
    default:
        ret_type = expr->type;
        break;
    }
    return ret_type;
}

void type_check_expr(ParsingContext *context, AstNode *expr) {
    AstNode *temp_expr = expr;
    int stat = -1;

    // AstNode *expr_child = expr->child;
    // while (expr_child != NULL) {
    //     type_check_expr(context, expr_child);
    //     expr_child = expr_child->next_child;
    // }

    switch (temp_expr->type) {
    case TYPE_BINARY_OPERATOR:;
        ParsingContext *temp_ctx = context;
        while (temp_ctx->parent_ctx != NULL)
            temp_ctx = temp_ctx->parent_ctx;
        AstNode *op_sym = create_node_symbol(temp_expr->ast_val.node_symbol);
        AstNode *op_data = get_env(temp_ctx->binary_ops, op_sym, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        int op_used_lhs_type = get_return_type(context, expr->child);
        AstNode *op_decl_lhs_type =
            parser_get_type(temp_ctx, op_data->child->next_child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->next_child->ast_val.node_symbol, 0);
        if (op_used_lhs_type != op_decl_lhs_type->type)
            print_error(ERR_COMMON, "Found Mismatched LHS type for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        int op_used_rhs_type = get_return_type(context, expr->child->next_child);
        AstNode *op_decl_rhs_type =
            parser_get_type(temp_ctx, op_data->child->next_child->next_child->next_child, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for type : `%s`",
                        op_data->child->next_child->next_child->next_child->ast_val.node_symbol, 0);
        if (op_used_rhs_type != op_decl_rhs_type->type)
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
        int param_call_type = -1;
        while (func_call_params != NULL && func_param_list != NULL) {
            param_list_type = parser_get_type(context, func_param_list->child->next_child, &stat);
            param_call_type = get_return_type(context, func_call_params);
            if (param_call_type == TYPE_NULL)
                break;
            if (param_call_type != param_list_type->type) {
                print_error(ERR_SYNTAX, "Mismatched argument type for function call : `%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            }
            func_param_list = func_param_list->next_child;
            func_call_params = func_call_params->next_child;
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

    while (temp_expr != NULL) {
        type_check_expr(context, temp_expr);
        temp_expr = temp_expr->next_child;
    }
}
