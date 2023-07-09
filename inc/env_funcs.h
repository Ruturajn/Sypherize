#ifndef __ENV_FUNCS_H__
#define __ENV_FUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AstNode AstNode;

/**
 * @brief Structure defining binding for an identifier, i.e. a node for
 *        the identifier, and it's value.
 */
typedef struct IdentifierBind {
    AstNode *identifier;                 ///< Node for the identifier.
    AstNode *id_val;                     ///< Node for the value.
    struct IdentifierBind *next_id_bind; ///< Pointer to the next binding.
} IdentifierBind;

/**
 * @brief Structure defining an environment which contains the binding, and
 *        a parent environment.
 */
typedef struct Env {
    IdentifierBind *binding; ///< Pointer to the binding.
    struct Env *parent_env;  ///< Pointer to the parent environment.
} Env;

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

#ifdef __cplusplus
}
#endif

#endif /* __ENV_FUNCS_H__ */
