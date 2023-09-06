#ifndef __ENV_FUNCS_H__
#define __ENV_FUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "parser.h"

/**
 * @brief  Creates a new environment.
 *
 * @param  parent_Env [`Env *`] Ponter to the parent environment
 *                    to which the new environment must be associated to.
 * @return Env*       Pointer to the newly created environment.
 */
Env *create_env(Env *parent_Env);

/**
 * @brief  Sets the environment with a identifier and its value.
 *
 * @param  Env_to_set      [`Env **`] Double pointer to the environment that
 *                         should contain the binding, for the identifier
 *                         and the value.
 * @param  identifier_node [`AstNode *`] Pointer to the identifier node.
 * @param  id_val          [`AstNode *`] Pointer to the value node.
 * @return int             `1` for success, and `0` for failure.
 */
int set_env(Env **Env_to_set, AstNode *identifier_node, AstNode *id_val);

/**
 * @brief  Gets the type node based on the identifier node.
 *
 * @param  Env_to_get [`Env *`] Pointer to the environment which
 *                    must be looked at.
 * @param  identifier [`AstNode *`] Pointer to the identifier node.
 * @param  stat       [`int`] Status of the function execution.
 * @return AstNode*   Pointer to the type node.
 */
AstNode *get_env(Env *Env_to_get, AstNode *identifier, int *stat);

AstNode *get_env_from_sym(Env *Env_to_get, char *identifier, int *stat);

AstNode *get_env_from_val(Env *Env_to_get, AstNode *val, int *stat);

/**
 * @brief  Searches the parsing contexts, for a type node based
 *         on the identifier.
 *
 * @param  Env_to_get [`ParsingContext *`] Pointer to the ParsingContext
 *                    context.
 * @param  identifier [`AstNode *`] Pointer to the identifier node.
 * @param  stat       [`int`] Status of the function execution.
 * @return AstNode*   Pointer to the type node.
 */
AstNode *parser_get_type(ParsingContext *context, AstNode *identifier,
                         int *stat);

AstNode *parser_get_var(ParsingContext *context, AstNode *identifier,
                        int *stat);
/**
 * @brief  Searches the parsing contexts, for a type func based
 *         on the identifier.
 *
 * @param  Env_to_get [`ParsingContext *`] Pointer to the ParsingContext
 *                    context.
 * @param  identifier [`AstNode *`] Pointer to the identifier node.
 * @param  stat       [`int`] Status of the function execution.
 * @return AstNode*   Pointer to the type node.
 */
AstNode *parser_get_func(ParsingContext *context, AstNode *identifier,
                         int *stat);

void print_env(Env *env, int indent);

#ifdef __cplusplus
}
#endif

#endif /* __ENV_FUNCS_H__ */
