#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LexedToken LexedToken;
typedef struct Env Env;

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
    TYPE_VAR_ACCESS,       ///< Node that represents variable access.
    TYPE_SYM,              ///< Node for a symbol.
    TYPE_VAR_REASSIGNMENT, ///< Node for variable reassignment.
    TYPE_FUNCTION,         ///< Node for storing functions.
    TYPE_FUNCTION_CALL,    ///< Node for storing function calls.
    TYPE_IF_CONDITION,     ///< Node for storing if-else statements.
    TYPE_ADDROF,           ///< Node for storing address of a variable.
    TYPE_DEREFERENCE,      ///< Node for storing the dereferenced value of a
                           ///< pointer.
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
    unsigned int pointer_level; ///< Indirection level for a pointer.
} AstNode;

typedef struct ParsingStack {
    struct ParsingStack *parent_stack; ///< Pointer to the parent stack.
    AstNode *op;                       ///< Pointer to an operator that caused the increase in
                                       ///< the stack.
    AstNode *res;                      ///< Pointer to an AstNode to keep track of the list
                                       ///< of expressions in a function body.
    AstNode *body;
} ParsingStack;

/**
 * @brief Structure defining a parsing context, which contains an environment
 *        with types, and variables. This enables scoped access to variables.
 */
typedef struct ParsingContext {
    struct ParsingContext *child;
    struct ParsingContext *next_child;
    struct ParsingContext *parent_ctx; ///< Pointer to the parent context.
    Env *env_type;                     ///< Pointer to an environment for types.
    Env *vars;                         ///< Pointer to an environment for varaibles.
    Env *funcs;                        ///< Pointer to an environment for functions.
    Env *binary_ops;                   ///< Pointer to an environment for binary operators.
} ParsingContext;

typedef enum StackOpRetVal {
    STACK_OP_BREAK = 0,
    STACK_OP_CONT_PARSE,
    STACK_OP_CONT_CHECK,
    STACK_OP_INVALID,
} StackOpRetVal;

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

ParsingStack *create_parsing_stack(ParsingStack *parent_stack);

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
void ast_add_type_node(Env **env_type, int node_type, AstNode *sym, long byte_size);

void ast_add_binary_ops(ParsingContext **context, char *bin_op, int precedence, char *ret_type,
                        char *lhs_type, char *rhs_type);

void add_parsing_context_child(ParsingContext **root, ParsingContext *child_to_add);

/**
 * @brief Lexes and parses a complete file.
 *
 * @param file_data   [`char *`] pointer to the data stream.
 */
void lex_and_parse(char *file_dest, ParsingContext **curr_context, AstNode **program);

int check_if_delims(LexedToken *token);

int check_invalid_var_access(ParsingContext *context, AstNode *sym_node);

void print_parsing_context(ParsingContext *context, int indent);

int parse_binary_infix_op(char **temp_file_data, LexedToken **curr_token, ParsingContext **context,
                          long *running_precedence, AstNode **curr_expr, AstNode **running_expr,
                          ParsingStack *curr_stack);

StackOpRetVal stack_operator_continue(ParsingStack **curr_stack, LexedToken **curr_token,
                                      AstNode **running_expr, char **temp_file_data,
                                      ParsingContext **context, long *running_precedence,
                                      AstNode **curr_expr);

AstNode *parse_type(AstNode **temp_type_node, LexedToken **curr_token, char **temp_file_data,
                    ParsingContext *context, int *status);

int check_if_type(char *temp_file_data, ParsingContext *context);
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
char *parse_tokens(char **temp_file_data, LexedToken *curr_token, AstNode **curr_expr,
                   ParsingContext **context);

#ifdef __cplusplus
}
#endif

#endif /* __PARSER_H__ */
