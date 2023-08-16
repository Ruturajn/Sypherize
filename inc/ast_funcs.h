#ifndef __AST_FUNCS_H__
#define __AST_FUNCS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lexer.h"
#include "parser.h"

#define NODE_BUF_SIZE 512

char *get_node_str(AstNode *node);

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
 * @brief  Compares two nodes.
 *
 * @param  node1 [`AstNode *`] Pointer to the first node.
 * @param  node2 [`AstNode *`] Pointer to the second node.
 * @return int   `1` to denote equality and `0` otherwise.
 */
int node_cmp(AstNode *node1, AstNode *node2);

/**
 * @brief Adds a child node to the parent in the AST.
 *
 * @param parent_node  [`AstNode *`] Pointer to the parent node.
 * @param child_to_add [`AstNode *`] Pointer to the child node
 *                     that needs to be added.
 */
void add_ast_node_child(AstNode *parent_node, AstNode *child_to_add);

/**
 * @brief  Allocates memory for a new node, and initialize its
 *         members.
 *
 * @return AstNode* Pointer to newly allocated node.
 */
AstNode *node_alloc();

/**
 * @brief  Creates a new node of type symbol from a string.
 *
 * @param  symbol_str [`char *`] Pointer to the symbol string.
 * @return AstNode*   Pointer to the newly created node.
 */
AstNode *create_node_symbol(char *symbol_str);

/**
 * @brief  Creates a new node of type TYPE_NULL.
 *
 * @return AstNode*   Pointer to the newly created node.
 */
AstNode *create_node_none();

/**
 * @brief  Creates a new node of type int from a value.
 *
 * @param  val       [`long`] Value for the node.
 * @return AstNode*  Pointer to the newly created node.
 */
AstNode *create_node_int(long val);

/**
 * @brief Frees nodes in an AST.
 *
 * @param node_to_free [`AstNode *`] Pointer to the node
 *                     that needs to be freed.
 */
void free_node(AstNode *node_to_free);

/**
 * @brief  Creates a node of type symbol from a token.
 *
 * @param  token     [`LexedToken *`] Pointer to the token.
 * @return AstNode*  Pointer to the newly created node.
 */
AstNode *node_symbol_from_token_create(LexedToken *token);

/**
 * @brief  Deep copies the content from the source node into
 *         the destination node.
 *
 * @param  dst_node [`AstNode *`] Pointer to the destination
 *                  node.
 * @param  src_node [`AstNode *`] Pointer to the source node.
 * @return int      `1` for success, and `0` for failure.
 */
int copy_node(AstNode *dst_node, AstNode *src_node);

#ifdef __cplusplus
}
#endif

#endif /* __AST_FUNCS_H__ */
