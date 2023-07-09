#ifndef __FILE_PROC_H__
#define __FILE_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../inc/helpers.h"

/**
 * @brief String containing all the the delimeters used
 *        for tokenizing.
 */
#define DELIMS " :=,;~()\r\n"

/**
 * @brief String containing all the the whitespace characters
 *        for tokenizing.
 */
#define WHITESPACE " ;\r\n"

/**
 * @brief String containing all the comment delimeters used
 *        for tokenizing.
 */
#define COMMENTS_DELIMS "`!"

/**
 * @brief Structure defining a token.
 */
typedef struct LexedToken {
    char *token_start; ///< Pointer to the beginning of the token.
    int token_length;  ///< Length of the token from the beginning.
} LexedToken;

/**
 * @brief Print out the token, pointed to by `curr_token`.
 *
 * @param curr_token   [`LexedToken *`] pointer to the token, that needs to be
 *                     printed.
 */
void print_lexed_token(LexedToken *curr_token);

/**
 * @brief  Compare the string present in a token, with another one.
 *
 * @param  curr_token  [`LexedToken *`] pointer to the token that needs
 *                     to be compared.
 * @param  str_to_cmp  [`char *`] pointer to the string that needs
 *                     to be compared.
 * @return int         `1` for success and `0` for failure.
 */
int strncmp_lexed_token(LexedToken *curr_token, char *str_to_cmp);

/**
 * @brief  Allocate a new token, with `token_length` and `data`.
 *
 * @param  token_length  [`int`] Length of the token from the `data`
 *                       pointer.
 * @param  data          [`char *`] pointer to the data that is being
 *                       lexed.
 * @return LexedToken*   Pointer to the newly created token.
 */
LexedToken *create_token(int token_length, char *data);

/**
 * @brief  Check whether a line is a comment.
 *
 * @param  file_data   [`char *`] Pointer to the file data stream.
 * @return int        `1` for success and `0` for failure.
 */
int check_comment(char *file_data);

/**
 * @brief  Create a new token from the file data stream.
 *
 * @param  file_data   [`char **`] double-pointer to the data stream.
 * @param  curr_token  [`LexedToken *`] pointer in which the new token
 *                     needs to be stored.
 * @return char*      Pointer to the data stream, after the current token.
 */
char *lex_token(char **file_data, LexedToken **curr_token);

/**
 * @brief  Lexes the next token, and checks whether it's equal to the
 *         string that is expected next. If it's not, the data stream
 *         is reset to it's previous state..
 *
 * @param  string_to_cmp  [`char *`] Pointer to the string, that needs
 *                        to be compared.
 * @param  temp_file_data [`char **`] Double pointer to the file data
 *                        stream.
 * @param  token          [`LexedToken *`] Pointer to a token, that
 *                        stores the next token.
 * @return int            `1` for success, and `0` for failure.
 */
int check_next_token(char *string_to_cmp, char **temp_file_data,
                     LexedToken **token);

#ifdef __cplusplus
}
#endif

#endif /* __FILE_PROC_H__ */
