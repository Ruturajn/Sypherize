#include "../inc/lexer.h"
#include "../inc/code_gen.h"
#include "../inc/parser.h"

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
    if (len != (int)strlen(str_to_cmp))
        return 0;
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
