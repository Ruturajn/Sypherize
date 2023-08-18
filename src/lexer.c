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

void lex_token(LexingState **state) {
    /// Tokenizing;
    int begin = 0;
    while (check_comment((*state)->file_data)) {
        (*state)->file_data = strpbrk((*state)->file_data, "\n");
        (*state)->file_data += (strspn((*state)->file_data, WHITESPACE));
    }
    if (*(*state)->file_data != '\0') {
        begin = strcspn((*state)->file_data, DELIMS);
        begin = (begin == 0 ? 1 : begin);
        (*state)->curr_token = create_token(begin, (*state)->file_data);
        (*state)->file_data += begin;
        (*state)->file_data += (strspn((*state)->file_data, WHITESPACE));
    } else {
        *(*state)->curr_token->token_start = '\0';
    }
}

int check_next_token(char *string_to_cmp, LexingState **state) {
    if (string_to_cmp == NULL || (*state)->file_data == NULL || (*state)->curr_token == NULL) {
        print_error(ERR_COMMON, "NULL pointer passed to `check_next_token()`", NULL, 0);
        return 0;
    }
    LexingState temp_state = **state;
    LexingState *temp_state_ptr = &temp_state;
    lex_token(&temp_state_ptr);
    if (strncmp_lexed_token(temp_state.curr_token, string_to_cmp)) {
        **state = temp_state;
        return 1;
    }
    return 0;
}
