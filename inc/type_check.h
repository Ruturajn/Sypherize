#ifndef __TYPE_CHECK_H__
#define __TYPE_CHECK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "parser.h"

int cmp_type(AstNode *node1, AstNode *node2);

int cmp_type_sym(AstNode *node1, AstNode *node2);

void print_type(AstNode *expr, AstNode *expected_type, AstNode *got_type);

void type_check_prog(ParsingContext *context, AstNode *prog);

AstNode *type_check_expr(ParsingContext *context, ParsingContext **context_to_enter, AstNode *expr);

#ifdef __cplusplus
}
#endif

#endif /* __TYPE_CHECK_H__ */
