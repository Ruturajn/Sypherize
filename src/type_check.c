#include "../inc/type_check.h"
#include "../inc/ast_funcs.h"

int get_return_type(ParsingContext *context, AstNode *expr) {
    int ret_type = -1;
    int stat = -1;
    switch (expr->type) {
    case TYPE_FUNCTION_CALL:;
        AstNode *func_body = parser_get_func(context, expr->child, &stat);
        if (!stat)
            print_error(ERR_COMMON,
                        "Function definition not found :"
                        "`%s`",
                        expr->child->ast_val.node_symbol, 0);
        ret_type = func_body->child->next_child->type;
        break;
    default:
        ret_type = expr->type;
        break;
    }
    return ret_type;
}

void type_check_expr(ParsingContext *context, AstNode *expr) {
    AstNode *temp_expr = expr;
    switch (temp_expr->type) {
    case TYPE_FUNCTION_CALL:;
        int stat = -1;
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
        for (;;) {
            if (func_call_params == NULL && func_param_list == NULL)
                break;
            if (func_call_params != NULL && func_param_list == NULL) {
                print_error(ERR_NUM_ARGS,
                            "Too many arguments passed to function : "
                            "`%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            }
            if (func_param_list != NULL && func_call_params == NULL) {
                print_error(ERR_NUM_ARGS,
                            "Expected more arguments for function : "
                            "`%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            }
            param_list_type = parser_get_type(
                context, func_param_list->child->next_child, &stat);
            param_call_type = get_return_type(context, func_call_params);
            if (param_call_type != param_list_type->type)
                print_error(ERR_SYNTAX,
                            "Mismatched type for function call : `%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            func_param_list = func_param_list->next_child;
            func_call_params = func_call_params->next_child;
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
