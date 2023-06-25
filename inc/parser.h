#ifndef __PARSER_H__
#define __PARSER_H__

#include "../inc/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure defining a node in the AST (Abstract Syntax Tree).
 */
typedef struct AstNode {
    /**
     * @brief Enumeration that defines a type for the `AstNode`.
     */
    enum NodeType {
        TYPE_NULL = 0,        ///< NULL type node.
        TYPE_PROGRAM,         ///< Node for the whole program.
        TYPE_ROOT,            ///< Node for the root of the program.
        TYPE_INT,             ///< Node of type integer.
        TYPE_BINARY_OPERATOR, ///< Node that represents a binary operator.
        TYPE_VAR_DECLARATION, ///< Node that represents a variable declaration.
        TYPE_VAR_INIT,        ///< Node that represents variable initialization.
        TYPE_SYM,             ///< Node for a symbol.
    } type;
    /**
     * @brief Union defining a collection for the value and the symbol (string).
     */
    union NodeVal {
        long val;          ///< Value for the node.
        char *node_symbol; ///< String representing the node (Identifier).
    } ast_val;
    struct AstNode *child;      ///< Child node for this node.
    struct AstNode *next_child; ///< Next child for this node.
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
    Env *env_type; ///< Pointer to an environment for types.
    Env *vars;     ///< Pointer to an environment for varaibles.
} ParsingContext;

/**
 * @brief This function prints out a node and all its children from the AST,
 *        pointed to by the `root_node`.
 *
 * @param root_node [`AstNode *`] Pointer to the node that needs to be
 *                  printed
 * @param indent    [int] The level of indentation required while printing.
 */
void print_ast_node(AstNode *root_node, int indent);

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
 * @brief Creates a new environment.
 *
 * @param parent_Env [`Env *`] Ponter to the parent environment
 *                   to which the new environment must be associated to.
 * @return Env*      Pointer to the newly created environment.
 */
Env *create_env(Env *parent_Env);

/**
 * @brief Sets the environment with a identifier and its value.
 *
 * @param Env_to_set      [`Env *`] Pointer to the environment that
 *                        should contain the binding, for the identifier
 *                        and the value.
 * @param identifier_node [`AstNode *`] Pointer to the identifier node.
 * @param id_val          [`AstNode *`] Pointer to the value node.
 * @return int
 */
int set_env(Env *Env_to_set, AstNode *identifier_node, AstNode *id_val);

/**
 * @brief Compares two nodes.
 *
 * @param node1 [`AstNode *`] Pointer to the first node.
 * @param node2 [`AstNode *`] Pointer to the second node.
 * @return int  `1` to denote equality and `0` otherwise.
 */
int node_cmp(AstNode *node1, AstNode *node2);

/**
 * @brief Gets the value node based on the identifier node.
 *
 * @param Env_to_get [`Env *`] Pointer to the environment which
 *                   must be looked at.
 * @param identifier [`AstNode *`] Pointer to the identifier node.
 * @param stat       [`int`] Status of the function execution.
 * @return AstNode*  Pointer to the value node.
 */
AstNode *get_env(Env *Env_to_get, AstNode *identifier, int *stat);

/**
 * @brief Adds a child node to the parent in the AST.
 *
 * @param parent_node  [`AstNode *`] Pointer to the parent node.
 * @param child_to_add [`AstNode *`] Pointer to the child node
 *                     that needs to be added.
 */
void add_ast_node_child(AstNode *parent_node, AstNode *child_to_add);

/**
 * @brief Creates a parsing context.
 *
 * @return ParsingContext* Pointer to the newly created parsing context.
 */
ParsingContext *create_parsing_context();

/**
 * @brief Allocates memory for a new node, and initialize its
 *        members.
 *
 * @return AstNode* Pointer to newly allocated node.
 */
AstNode *node_alloc();

/**
 * @brief Creates a new node of type symbol from a string.
 *
 * @param symbol_str [`char *`] Pointer to the symbol string.
 * @return AstNode*  Pointer to the newly created node.
 */
AstNode *node_symbol_create(char *symbol_str);

/**
 * @brief Creates a new node of type int from a value.
 *
 * @param val       [`long`] Value for the node.
 * @return AstNode* Pointer to the newly created node.
 */
AstNode *node_int_create(long val);

/**
 * @brief Frees nodes in an AST.
 *
 * @param node_to_free [`AstNode *`] Pointer to the node
 *                     that needs to be freed.
 */
void free_node(AstNode *node_to_free);

/**
 * @brief Creates a node of type symbol from a token.
 *
 * @param token     [`LexedToken *`] Pointer to the token.
 * @return AstNode* Pointer to the newly created node.
 */
AstNode *node_symbol_from_token_create(LexedToken *token);

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
 * @param context        [`ParsingContext *`] Pointer to the current
 *                       context.
 * @return char*         Pointer to the file data stream.
 */
char *parse_tokens(char **temp_file_data, LexedToken *curr_token,
                   AstNode **curr_expr, ParsingContext *context);

#endif /* __PARSER_H__ */
