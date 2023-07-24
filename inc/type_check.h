#ifndef __TYPE_CHECK_H__
#define __TYPE_CHECK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ParsingContext ParsingContext;
typedef struct AstNode AstNode;

int cmp_type(AstNode *node1, AstNode *node2);

AstNode *get_return_type(ParsingContext *context, ParsingContext **context_to_enter, AstNode *expr);

void type_check_prog(ParsingContext *context, AstNode *prog);

void type_check_expr(ParsingContext *context, ParsingContext **context_to_enter, AstNode *expr);

#ifdef __cplusplus
}
#endif

#endif /* __TYPE_CHECK_H__ */
