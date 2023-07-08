#include "../inc/lexer.h"
#include "../inc/code_gen.h"
#include "../inc/parser.h"

const char *err_strings[] = {"GENERIC",     "NUM ARGS",     "MEMORY ALLOCATION",
                             "FILE ACCESS", "FILE SIZE",    "FILE READ",
                             "SYNTAX",      "REDEFINITION", "EOF"};

size_t calculate_file_size(FILE *file_ptr) {
    if (file_ptr == NULL) {
        return 0;
    }

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    long file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    if (file_sz < 0)
        print_error(ERR_FILE_SIZE, "Could not calculate file size", NULL, 0);

    return file_sz;
}

void print_lexed_token(LexedToken *curr_token) {
    if (curr_token == NULL)
        return;
    printf("%.*s", curr_token->token_length, curr_token->token_start);
}

int strncmp_lexed_token(LexedToken *curr_token, char *str_to_cmp) {
    if (curr_token == NULL || str_to_cmp == NULL) {
        return 0;
    }
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

LexedToken *create_token(int token_length, char *data) {
    LexedToken *curr_token = (LexedToken *)calloc(1, sizeof(LexedToken));
    CHECK_NULL(curr_token, "Could not allocate memory for token", NULL);
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

char *lex_token(char **file_data, LexedToken **curr_token) {
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
    *file_data += begin;
    *file_data += (strspn(*file_data, WHITESPACE));

    return *file_data;
}

void print_error(ErrType err, char *fmt, char *str, int val) {
    printf("\033[1;31m[ERROR]\033[1;37m ");
    printf("%s :: ", err_strings[err]);
    if (val != 0)
        printf((const char *)fmt, val);
    else {
        if (str != NULL)
            printf((const char *)fmt, str);
        else
            printf("%s", fmt);
    }
    printf("!\n");
    exit(EXIT_FAILURE);
}

int check_next_token(char *string_to_cmp, char **temp_file_data,
                     LexedToken **token) {
    if (string_to_cmp == NULL || *temp_file_data == NULL || token == NULL) {
        print_error(ERR_COMMON, "NULL pointer passed to `check_next_token()`",
                    NULL, 0);
        return 0;
    }
    char *prev_file_data = *temp_file_data;
    LexedToken *temp_token = *token;
    prev_file_data = lex_token(&prev_file_data, &temp_token);
    if (strncmp_lexed_token(temp_token, string_to_cmp)) {
        *token = temp_token;
        *temp_file_data = prev_file_data;
        return 1;
    }
    return 0;
}

char *read_file_data(char *file_dest) {
    FILE *file_ptr = NULL;
    file_ptr = fopen(file_dest, "r");
    CHECK_NULL(file_ptr, "Unable to open file : `%s`", file_dest);

    size_t file_sz = calculate_file_size(file_ptr);

    char *file_data = NULL;

    file_data = (char *)calloc((file_sz + 1), sizeof(char));
    CHECK_NULL(file_data, "Unable to allocate memory for file contents", NULL);

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error(ERR_FILE_READ, "Unable to read file contents : `%s`",
                    file_dest, 0);

    file_data[file_sz] = '\0';
    fclose(file_ptr);
    return file_data;
}
