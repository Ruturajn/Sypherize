#include "../inc/parser.h"

void print_ast_nodes_type(ast_nodes *curr_node) {
    if (curr_node == NULL) { return; }
    switch(curr_node->type) {
        case TYPE_ROOT:
            printf("ROOT\n");
            break;
        case TYPE_INT:
            printf("INT : %ld\n", curr_node->node_val);
            break;
        case TYPE_NULL:
            printf("NULL\n");
            break;
        default:
            printf("Unknown TYPE\n");
            break;
    }
}

void print_ast_nodes(ast_nodes *root_node, int indent) {
    if (root_node == NULL) { return; }
    for (int i=0; i < indent; i++)
        putchar(' ');
    print_ast_nodes_type(root_node);
    putchar('\n');
    ast_nodes *child_node = root_node->child;
    while (child_node != NULL) {
        print_ast_nodes(child_node, indent + 4);
        child_node = child_node->next_child;
    }
}

int parse_int(lexed_tokens *token, ast_nodes *node) {
    if (token == NULL || node == NULL) { return 2; }
    if (token->token_length == 0 && *(token->token_start) == '0') {
        node->type = TYPE_INT;
        node->node_val = 0;
    } else {
        long temp = strtol(token->token_start, NULL, 10);
        if (temp != 0) {
            node->type = TYPE_INT;
            node->node_val = temp;
        }
        else { return 0; }
    }
    return 1;
}

void parse_tokens(lexed_tokens *curr_token, ast_nodes curr_node) {

    if (parse_int(curr_token, &curr_node)) {
        // If this is an integer, look for a valid operator.
        printf("Found integer\n");
    } else {
        // If the token was not an integer, check if it is
        // variable declaration, assignment, etc.
        printf("Not an Integer\n");
    }

    // add_token_to_ll(curr_token, &root_token);
    print_ast_nodes(&curr_node, 0);
}
