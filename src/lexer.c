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

int check_comment(char *file_data) {
    char *temp = COMMENTS_DELIMS;
    while (*temp != '\0') {
        if (*file_data == *temp)
            return 1;
        temp++;
    }
    return 0;
}

char* lex_token(char **file_data, LexedToken **curr_token) {
    /// Tokenizing;
    int begin = 0;
    while (check_comment(*file_data)) {
        *file_data = strpbrk(*file_data, "\n");
        *file_data += (strspn(*file_data, WHITESPACE));
    }
    if (**file_data == '\0')
        return *file_data;
    begin = strcspn(*file_data, DELIMS);
    begin = (begin == 0 ? 1 : begin);
    *curr_token = create_token(begin, *file_data);
    print_lexed_token(*curr_token);
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

    LexedToken *root_token = NULL;
    LexedToken *curr_token = root_token;

    // The whole program will be tokenized and parsed into an
    // AST which will be created from the curr_node.
    AstNode *curr_expr;

    AstNode *program = node_alloc();
    program->type = TYPE_PROGRAM;

    ParsingContext *curr_context = create_parsing_context();

    while (*temp_file_data != '\0') {
        curr_expr = node_alloc();

        temp_file_data = parse_tokens(&temp_file_data, curr_token, &curr_expr, curr_context);
        if (curr_expr->type != TYPE_NULL)
            add_ast_node_child(program, curr_expr);
        free_node(curr_expr);
    }
    print_ast_node(program, 0);

    free(file_data);
    fclose(file_ptr);
}
