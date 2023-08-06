#include "../inc/type_check.h"
#include "../inc/ast_funcs.h"
#include "../inc/env_funcs.h"
#include "../inc/helpers.h"
#include "../inc/parser.h"

int cmp_type(AstNode *node1, AstNode *node2) {
    if ((node1->type != node2->type) || (node1->pointer_level != node2->pointer_level))
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

int cmp_type_sym(AstNode *node1, AstNode *node2) {
    if ((node1->type != node2->type) || (node1->pointer_level != node2->pointer_level) ||
        ((node1->ast_val.node_symbol != NULL && node2->ast_val.node_symbol != NULL) &&
         (strcmp(node1->ast_val.node_symbol, node2->ast_val.node_symbol) != 0)))
        return 0;
    return 1;
}

void print_type(AstNode *expr, AstNode *expected_type, AstNode *got_type) {
    printf("\n\nEXPRESSION:\n");
    print_ast_node(expr, 0);
    if (expected_type != NULL) {
        printf("EXPECTED TYPE:");
        for (unsigned int i = 0; i < expected_type->pointer_level; i++) {
            putchar('@');
        }
        printf("%s\n", expected_type->ast_val.node_symbol);
    }
    printf("FOUND TYPE:");
    for (unsigned int i = 0; i < got_type->pointer_level; i++) {
        putchar('@');
    }
    printf("%s\n\n", got_type->ast_val.node_symbol);
}

AstNode *type_check_expr(ParsingContext *context, ParsingContext **context_to_enter,
                         AstNode *expr) {
    AstNode *temp_expr = expr;
    AstNode *ret_type = node_alloc();
    int stat = -1;

    switch (temp_expr->type) {
    case TYPE_INT:;
        AstNode *ret_int_type = create_node_symbol("int");
        *ret_type = *ret_int_type;
        break;
    case TYPE_ADDROF:;
        if (temp_expr->child == NULL || temp_expr->child->type != TYPE_VAR_ACCESS) {
            print_type(temp_expr, NULL, temp_expr->child);
            print_error(ERR_SYNTAX, "Expected valid variable access for AddressOf operator", NULL,
                        0);
        }
        AstNode *var_access_type = type_check_expr(context, context_to_enter, temp_expr->child);
        *ret_type = *var_access_type;
        ret_type->pointer_level += 1;
        break;
    case TYPE_VAR_ACCESS:;
        ParsingContext *temp_ctx = context;
        AstNode *sym_type = NULL;
        while (temp_ctx != NULL) {
            sym_type = get_env_from_sym(temp_ctx->vars, expr->ast_val.node_symbol, &stat);
            if (stat)
                break;
            temp_ctx = temp_ctx->parent_ctx;
            if (temp_ctx != NULL)
                free_node(sym_type);
        }
        if (stat == 0)
            print_error(ERR_COMMON, "Couldn't find information for variable : `%s`",
                        expr->ast_val.node_symbol, 0);

        *ret_type = *sym_type;
        break;
    case TYPE_DEREFERENCE:;
        AstNode *deref_type = type_check_expr(context, context_to_enter, temp_expr->child);
        if (deref_type->pointer_level == 0) {
            print_type(temp_expr, NULL, temp_expr->child);
            print_error(ERR_TYPE, "Only pointer types can be dereferenced", NULL, 0);
        }
        deref_type->pointer_level -= 1;
        *ret_type = *deref_type;
        break;
    case TYPE_IF_CONDITION:;
        // The last expression of both the if-then body and the else body needs
        // to be checked. The last expression in both the bodies should be of
        // the same type.

        // Type check the condition of the if statement.
        AstNode *if_cond = type_check_expr(context, context_to_enter, temp_expr->child);
        free_node(if_cond);

        // Type check the IF body.
        AstNode *if_body_expr = temp_expr->child->next_child->child;
        AstNode *if_expr_ret_type = NULL;
        while (if_body_expr != NULL) {
            if_expr_ret_type = type_check_expr(context, context_to_enter, if_body_expr);
            if (if_body_expr->next_child != NULL)
                free_node(if_expr_ret_type);
            if_body_expr = if_body_expr->next_child;
        }

        // Turns out doing this doesn't cause any issues for empty IF bodies,
        // because there is NULL node added to the body.
        if (if_expr_ret_type == NULL)
            print_error(ERR_TYPE,
                        "No return type found for the last expression in the IF-THEN body", NULL,
                        0);

        // Check if there is a ELSE body, if so type check it, and compare the
        // last expressions of both the IF body and the ELSE body.
        if (temp_expr->child->next_child->next_child != NULL) {
            AstNode *else_body_expr = temp_expr->child->next_child->next_child->child;
            AstNode *else_expr_ret_type = NULL;
            while (else_body_expr != NULL) {
                else_expr_ret_type = type_check_expr(context, context_to_enter, else_body_expr);
                if (else_body_expr->next_child != NULL)
                    free_node(else_expr_ret_type);
                else_body_expr = else_body_expr->next_child;
            }

            if (cmp_type_sym(if_expr_ret_type, else_expr_ret_type) == 0) {
                print_type(temp_expr, if_expr_ret_type, else_expr_ret_type);
                print_error(ERR_TYPE, "IF-THEN body and the ELSE body do not return the same type",
                            NULL, 0);
            }
        }
        *ret_type = *if_expr_ret_type;
        break;
    case TYPE_FUNCTION:;
        ParsingContext *to_enter = (*context_to_enter)->child;
        AstNode *function_body = temp_expr->child->next_child->next_child->child;
        AstNode *expr_type = NULL;
        while (function_body != NULL) {
            expr_type = type_check_expr(*context_to_enter, &to_enter, function_body);
            function_body = function_body->next_child;
            if (function_body != NULL)
                free_node(expr_type);
        }

        copy_node(ret_type, temp_expr->child->next_child);

        if (stat == 0)
            print_error(ERR_COMMON, "Couldn't find information for return type of the function",
                        NULL, 0);
        if (cmp_type_sym(expr_type, ret_type) == 0) {
            print_type(temp_expr, ret_type, expr_type);
            print_error(ERR_TYPE,
                        "Found Mismatched type for function return type and last expression", NULL,
                        0);
        }

        (*context_to_enter) = (*context_to_enter)->next_child;
        break;
    case TYPE_VAR_REASSIGNMENT:;
        // Get the return type of the left hand side of a variable declaration.
        AstNode *lhs_ret_type = type_check_expr(context, context_to_enter, temp_expr->child);
        // Get the return type of the left hand side of a variable declaration.
        AstNode *rhs_ret_type =
            type_check_expr(context, context_to_enter, temp_expr->child->next_child);
        if (cmp_type_sym(lhs_ret_type, rhs_ret_type) == 0) {
            print_type(temp_expr, lhs_ret_type, rhs_ret_type);
            free_node(rhs_ret_type);
            print_error(ERR_TYPE, "Mismatched types for variable re-assignment", NULL, 0);
        }
        ret_type = lhs_ret_type;
        free_node(rhs_ret_type);
        break;
    case TYPE_BINARY_OPERATOR:;
        temp_ctx = context;
        while (temp_ctx->parent_ctx != NULL)
            temp_ctx = temp_ctx->parent_ctx;
        AstNode *op_sym = create_node_symbol(temp_expr->ast_val.node_symbol);
        AstNode *op_data = get_env(temp_ctx->binary_ops, op_sym, &stat);
        if (!stat)
            print_error(ERR_COMMON, "Couldn't find information for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);

        AstNode *op_used_lhs_type = type_check_expr(context, context_to_enter, expr->child);
        AstNode *op_decl_lhs_type = op_data->child->next_child->next_child;
        if (cmp_type_sym(op_used_lhs_type, op_decl_lhs_type) == 0) {
            print_type(temp_expr, op_decl_lhs_type, op_used_lhs_type);
            print_error(ERR_COMMON, "Found Mismatched LHS type for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);
        }

        AstNode *op_used_rhs_type =
            type_check_expr(context, context_to_enter, expr->child->next_child);
        AstNode *op_decl_rhs_type = op_data->child->next_child->next_child->next_child;
        if (cmp_type_sym(op_used_rhs_type, op_decl_rhs_type) == 0) {
            print_type(temp_expr, op_decl_rhs_type, op_used_rhs_type);
            print_error(ERR_COMMON, "Found Mismatched RHS type for operator : `%s`",
                        temp_expr->ast_val.node_symbol, 0);
        }

        free_node(op_sym);
        free_node(op_data);
        free_node(op_decl_lhs_type);
        free_node(op_decl_rhs_type);
        AstNode *op_decl_ret_type = op_data->child->next_child;
        *ret_type = *op_decl_ret_type;
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
        while (func_call_params != NULL && func_param_list != NULL) {
            param_call_type = type_check_expr(context, context_to_enter, func_call_params);

            AstNode *complete_param_list_type = node_alloc();
            copy_node(complete_param_list_type, func_param_list->child->next_child);

            if (stat == 0)
                print_error(ERR_TYPE, "Unable to find type information for : `%s`",
                            func_param_list->child->next_child->ast_val.node_symbol, 0);
            if (param_call_type->type == TYPE_NULL)
                break;
            if (cmp_type_sym(param_call_type, complete_param_list_type) == 0) {
                print_type(temp_expr, complete_param_list_type, param_call_type);
                print_error(ERR_SYNTAX, "Mismatched argument type for function call : `%s`",
                            temp_expr->child->ast_val.node_symbol, 0);
            }
            func_param_list = func_param_list->next_child;
            func_call_params = func_call_params->next_child;
            free_node(complete_param_list_type);
            free_node(param_call_type);
            free_node(param_list_type);
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
        stat = -1;
        AstNode *final_function_return_type = node_alloc();
        copy_node(final_function_return_type, func_body->child->next_child);
        *ret_type = *final_function_return_type;
        free_node(func_body);
        break;
    default:
        break;
    }
    return ret_type;
}

void type_check_prog(ParsingContext *context, AstNode *prog) {
    AstNode *temp_expr = prog->child;

    ParsingContext *context_to_enter = context->child;

    while (temp_expr != NULL) {
        type_check_expr(context, &context_to_enter, temp_expr);
        temp_expr = temp_expr->next_child;
    }
}
