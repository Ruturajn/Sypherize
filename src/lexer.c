#include "../inc/lexer.h"

long calculate_file_size(FILE *file_ptr) {
    if (file_ptr == NULL) { return 0; }

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error("Unable to access file");

    long file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (file_sz < 0)
        print_error("Unable to get file size");

    printf("The file size is : %ld\n", file_sz);

    return file_sz;
}

void add_token_to_ll(lexed_tokens *new_token, lexed_tokens **root_token){
    if (*root_token == NULL)
        *root_token = new_token;
    else {
        lexed_tokens *temp = *root_token;
        while (temp->next_token != NULL)
            temp = temp->next_token;
        temp->next_token = new_token;
    }
}

void print_lexed_tokens_ll(lexed_tokens *root_token) {
    if (root_token == NULL) { return; }
    lexed_tokens *temp = root_token;
    while (temp != NULL) {
        printf("Token : %.*s\n", temp->token_length, temp->token_start);
        temp = temp->next_token;
    }
}

void free_lexed_tokens_ll(lexed_tokens **root_token) {
    if (root_token == NULL) { return; }
    lexed_tokens *temp = *root_token;
    lexed_tokens *temp_next = NULL;
    while (temp != NULL) {
        temp_next = temp->next_token;
        free(temp);
        temp = temp_next;
    }
}

int strncmp_lexed_tokens(lexed_tokens *curr_token, char *str_to_cmp) {
    if (curr_token == NULL || str_to_cmp == NULL) { return 0; }
    int len = curr_token->token_length;
    char *temp = curr_token->token_start;
    while (len != 0) {
        if (*temp != *str_to_cmp)
            return 0;
        temp++;
        str_to_cmp++;
        len--;
    }
    return 1;
}

int parse_int(lexed_tokens *token, ast_nodes *node) {
    if (token == NULL || node == NULL) { return 0; }
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

lexed_tokens* create_token(int token_length, char *data) {
    lexed_tokens *curr_token = (lexed_tokens *) calloc(1, sizeof(lexed_tokens));
    CHECK_NULL(curr_token, "Unable to allocate memory");
    curr_token->token_length = token_length;
    curr_token->token_start = data;
    curr_token->next_token = NULL;

    return curr_token;
}

char* lex_token(char **file_data, lexed_tokens **curr_token) {
    /// Tokenizing;
    int begin = 0;
    begin = strcspn(*file_data, DELIMS);
    begin = (begin == 0 ? 1 : begin);
    *curr_token = create_token(begin, *file_data);
    *file_data += begin;
    *file_data += (strspn(*file_data, WHITESPACE));

    return *file_data;
}

void lex_file(char *file_dest) {

    FILE *file_ptr = NULL;
    file_ptr = fopen(file_dest, "r");
    CHECK_NULL(file_ptr, "Unable to open file");

    long file_sz = calculate_file_size(file_ptr);

    char *file_data = NULL;

    file_data = (char *)malloc((file_sz + 1) * sizeof(char));
    CHECK_NULL(file_data, "Unable to allocate memory");

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error("Unable to read from file");

    file_data[file_sz] = '\0';

    char *temp_file_data = file_data;
    temp_file_data += strspn(temp_file_data, WHITESPACE);
    size_t total_tokens = 0;

    lexed_tokens *root_token = NULL;
    lexed_tokens *curr_token = root_token;

    while (*temp_file_data != '\0') {
        temp_file_data = lex_token(&temp_file_data, &curr_token);
        add_token_to_ll(curr_token, &root_token);
        total_tokens += 1;
    }

    print_lexed_tokens_ll(root_token);

    // lexed_tokens *temp_token = root_token;
    // while (temp_token) {
    //     if (strncmp_lexed_tokens(temp_token, ":") == 1) {
    //         if (temp_token->next_token != NULL &&
    //                 strncmp_lexed_tokens(temp_token->next_token, "=") == 1)
    //             printf("Found variable assignment\n");
    //         else
    //             printf("Found Function parameter\n");
    //     }
    //     // TODO: Make sure, that an identifer is present after the type name and
    //     // that it is a valid variable declaration.
    //     if (strncmp_lexed_tokens(temp_token, "int") == 1) {
    //         if (temp_token->next_token != NULL ) {
    //             if (strncmp_lexed_tokens(temp_token->next_token, ":") != 1 &&
    //                 strncmp_lexed_tokens(temp_token->next_token, "{") != 1)
    //                 printf("Found a variable declaration\n");
    //         }
    //     }
    // }

    free_lexed_tokens_ll(&root_token);
    free(file_data);
    fclose(file_ptr);
}
