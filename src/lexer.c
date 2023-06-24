#include "../inc/lexer.h"
#include "../inc/parser.h"

long calculate_file_size(FILE *file_ptr) {
    if (file_ptr == NULL) { return 0; }

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(FILE_OPEN_ERR, 1);

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error(FILE_OPEN_ERR, 1);

    long file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(FILE_OPEN_ERR, 1);

    if (file_sz < 0)
        print_error(FILE_SIZE_ERR, 1);

    return file_sz;
}

void print_lexed_token(LexedToken *curr_token) {
    if (curr_token == NULL) { return; }
    printf("Token : %.*s\n", curr_token->token_length, curr_token->token_start);
}

int strncmp_lexed_token(LexedToken *curr_token, char *str_to_cmp) {
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

LexedToken* create_token(int token_length, char *data) {
    LexedToken *curr_token = (LexedToken *) calloc(1, sizeof(LexedToken));
    CHECK_NULL(curr_token, MEM_ERR);
    curr_token->token_length = token_length;
    curr_token->token_start = data;

    return curr_token;
}

char* lex_token(char **file_data, LexedToken **curr_token) {
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
    CHECK_NULL(file_data, MEM_ERR);

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error(FILE_OPEN_ERR, 1);

    file_data[file_sz] = '\0';

    char *temp_file_data = file_data;
    temp_file_data += strspn(temp_file_data, WHITESPACE);
    size_t total_tokens = 0;

    LexedToken *root_token = NULL;
    LexedToken *curr_token = root_token;

    // The whole program will be tokenized and parsed into an
    // AST which will be created from the curr_node.
    AstNode *curr_node = node_alloc();

    AstNode *program = node_alloc();
    program->type = TYPE_PROGRAM;
    add_ast_node_child(program, curr_node);

    ParsingContext *curr_context = create_parsing_context();

    while (*temp_file_data != '\0') {

        temp_file_data = parse_tokens(&temp_file_data, curr_token, curr_node, curr_context);

        curr_node->type = TYPE_NULL;
        curr_node->child = NULL;
        curr_node->next_child = NULL;

        total_tokens += 1;
    }

    // add_ast_node_child(program, curr_node);

    free(file_data);
    fclose(file_ptr);
}
