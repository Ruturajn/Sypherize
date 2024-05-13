#ifndef __LEXER_H__
#define __LEXER_H__

#include "./token.h"
#include <sys/types.h>
#include <vector>

class Lexer {
public:
    std::vector<Token> tok_list;
    std::string file_data;
    ssize_t curr_pos;
    ssize_t l_num;
    ssize_t col_num;
    ssize_t data_sz;
    static const std::vector<int> tok_length;

    Lexer(std::string _file_data):
        tok_list({}), file_data(std::move(_file_data)), curr_pos(0),
        l_num(1), col_num(1), data_sz(0) { data_sz = file_data.size(); }

    bool is_space(char c) const { return (c == ' ' || c == '\r'); }
    bool is_num(char c) const { return (c >= '0' && c <= '9'); }
    bool is_alpha(char c) const {
        return ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c >= 'Z') || (c == '_'));
    }

    char check_next();
    bool skip_until_newline();
    bool skip_until_comment_close();
    Token create_token(ssize_t tok_sz, enum Token::TokType t_ty);
    void push_tok(ssize_t tok_sz, enum Token::TokType t_ty);
    void lex_number();
    void lex_identifier();
    void lex();
    void print_tokens() const;
};

#endif // __LEXER_H__
