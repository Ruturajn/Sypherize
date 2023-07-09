#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../inc/lexer.h"

/**
 * @brief Enumeration that defines a type for the `AstNode`.
 */
typedef enum NodeType {
    TYPE_NULL = 0,         ///< NULL type node.
    TYPE_PROGRAM,          ///< Node for the whole program.
    TYPE_ROOT,             ///< Node for the root of the program.
    TYPE_INT,              ///< Node of type integer.
    TYPE_BINARY_OPERATOR,  ///< Node that represents a binary operator.
    TYPE_VAR_DECLARATION,  ///< Node that represents a variable declaration.
    TYPE_VAR_INIT,         ///< Node that represents variable initialization.
    TYPE_SYM,              ///< Node for a symbol.
    TYPE_VAR_REASSIGNMENT, ///< Node for variable reassignment.
    TYPE_FUNCTION,         ///< Node for storing functions.
    TYPE_FUNCTION_CALL,    ///< Node for storing function calls.
} NodeType;

/**
 * @brief Structure defining the value for an AST Node.
 */
typedef struct NodeVal {
    long val;          ///< Value for the node.
    char *node_symbol; ///< String representing the node (Identifier).
} NodeVal;

/**
 * @brief Structure defining a node in the AST (Abstract Syntax Tree).
 */
typedef struct AstNode {
    int type;                   ///< Enum for the type of the node.
    NodeVal ast_val;            ///< Struct for storing the value
                                ///< of the node.
    struct AstNode *child;      ///< Child node for this node.
    struct AstNode *next_child; ///< Next child for this node.
    int result_reg_desc;        ///< Register descriptor for stroing the
                                ///< rersult.
} AstNode;

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
 * @brief Structure defining a parsing context, which contains an environment
 *        with types, and variables. This enables scoped access to variables.
 */
typedef struct ParsingContext {
    struct ParsingContext *parent_ctx; ///< Pointer to the parent context.
    AstNode *op;     ///< Pointer to an operator that caused the increase in
                     ///< the stack.
    AstNode *res;    ///< Pointer to an AstNode to keep track of the list
                     ///< of expressions in a function body.
    Env *env_type;   ///< Pointer to an environment for types.
    Env *vars;       ///< Pointer to an environment for varaibles.
    Env *funcs;      ///< Pointer to an environment for functions.
    Env *binary_ops; ///< Pointer to an environment for binary operators.
} ParsingContext;

/**
 * @brief Parses an integer from a token, and stores it in
 *        a node.
 *
 * @param token [`LexedToken *`] Pointer to the token that needs
 *              to be parsed.
 * @param node  [`AstNode *`] Pointer to the node, in which the
 *              integer needs to be stored.
 * @return int  `1` for success, and `0` for failure.
 */
int parse_int(LexedToken *token, AstNode *node);

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

/**
 * @brief  Creates a parsing context, without any types.
 *
 * @param  [`ParsingContext *`] Pointer to the parent context.
 * @return ParsingContext* Pointer to the newly created parsing context.
 */
ParsingContext *create_parsing_context(ParsingContext *parent_ctx);

/**
 * @brief  Creates a default parsing context, with all the types.
 *
 * @return ParsingContext* Pointer to the newly created parsing context.
 */
ParsingContext *create_default_parsing_context();

/**
 * @brief Adds a new type with a binding to the types environment.
 *
 * @param env_type  [`Env **`] Double-pointer to the environment.
 * @param node_type [`int`] Enum value for the node type.
 * @param sym       [`AstNode *`] Symbol for the new type.
 * @param byte_size [`long`] Size in bytes of the new type.
 */
void ast_add_type_node(Env **env_type, int node_type, AstNode *sym,
                       long byte_size);

void ast_add_binary_ops(ParsingContext **context, char *bin_op, int precedence,
                        char *ret_type, char *lhs_type, char *rhs_type);

/**
 * @brief Lexes and parses a complete file.
 *
 * @param file_data   [`char *`] pointer to the data stream.
 */
void lex_and_parse(char *file_dest, ParsingContext **curr_context,
                   AstNode **program);

int parse_binary_infix_op(char **temp_file_data, LexedToken **curr_token,
                          ParsingContext **context, long *running_precedence,
                          AstNode **curr_expr, AstNode **running_expr);

/**
 * @brief Parses tokens (TODO!!), and advances the pointer that
 *        points to the file data stream.
 *
 * @param temp_file_data [`char **`] Double-pointer to the
 *                       file data stream.
 * @param curr_token     [`LexedToken *`] Pointer in which the next
 *                       token is stored.
 * @param curr_expr      [`AstNode **`] Pointer to the node which
 *                       stores the current expression.
 * @param context        [`ParsingContext **`] Double pointer to
 *                       the current context.
 * @return char*         Pointer to the file data stream.
 */
char *parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode **curr_expr, ParsingContext **context);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_H__ */
