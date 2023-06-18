#include "../inc/lexer.h"

lexed_tokens *root_token = NULL;

long calculate_file_size(FILE *file_ptr) {

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

void add_token_to_ll(int token_length, lexed_tokens **root_token,
                     char *data) {
    lexed_tokens *new_token = (lexed_tokens *) calloc(1, sizeof(lexed_tokens));
    CHECK_NULL(new_token, "Unable to allocate memory");
    new_token->token_length = token_length;
    new_token->token_name = (char *) calloc(new_token->token_length + 1, 
                                            sizeof(char));
    CHECK_NULL(new_token->token_name, "Unable to allocate memory");
    memcpy(new_token->token_name, data, token_length);
    new_token->token_name[token_length] = '\0';
    new_token->next_token = NULL;

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
    lexed_tokens *temp = root_token;
    while (temp != NULL) {
        printf("Token : %s\n", temp->token_name);
        temp = temp->next_token;
    }
}

void free_lexed_tokens_ll(lexed_tokens **root_token) {
    lexed_tokens *temp = *root_token;
    lexed_tokens *temp_next = NULL;
    while (temp != NULL) {
        temp_next = temp->next_token;
        free(temp->token_name);
        free(temp);
        temp = temp_next;
    }
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

    /// Tokenizing;
    char delims[20] = " :=,;~()\r\n";
    char whitespace[5] = " \r\n";

    int begin = 0;
    int end = 0;
    char *temp_file_data = file_data;
    begin = strspn(temp_file_data, whitespace);
    temp_file_data += begin;

    while (*temp_file_data != '\0') {
        begin = strcspn(temp_file_data, delims);
        if (begin < 1) {
            if (*temp_file_data != ' ' && *temp_file_data != '\n') {
                add_token_to_ll(1, &root_token, temp_file_data);
            }
            temp_file_data +=1 ;
        }
        else {
            add_token_to_ll(begin, &root_token, temp_file_data);
        }
        end = strspn(temp_file_data, whitespace);
        temp_file_data += (begin + end);
    }

    print_lexed_tokens_ll(root_token);

    lexed_tokens *temp_token = root_token;
    while (temp_token) {
        if (strcmp(temp_token->token_name, ":") == 0) {
            if (temp_token->next_token != NULL &&
                    strcmp(temp_token->next_token->token_name, "=") == 0)
                printf("Found variable assignment\n");
            else
                printf("Found Function parameter\n");
        }
        // TODO: Make sure, that an identifer is present after the type name and
        // that it is a valid variable declaration.
        if (strcmp(temp_token->token_name, "int") == 0) {
            if (temp_token->next_token != NULL ) {
                if (strcmp(temp_token->next_token->token_name, ":") != 0 &&
                    strcmp(temp_token->next_token->token_name, "{") != 0)
                    printf("Found a variable declaration\n");
            }
        }
        temp_token = temp_token->next_token;
    }

    free_lexed_tokens_ll(&root_token);
    free(file_data);
    fclose(file_ptr);
}
