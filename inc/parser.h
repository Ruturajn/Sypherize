#ifndef __PARSER_H__
#define __PARSER_H__

#include "./token.h"
#include <unordered_map>
#include <vector>

class Parser {
public:
    std::vector<Token> tok_list;
    ssize_t curr_pos;
    ssize_t tok_len;
    std::unordered_map<Token::TokType, int> precedence;

    Parser(std::vector<Token>& _tok_list)
        : tok_list(_tok_list), curr_pos(0), tok_len(_tok_list.size()) {
        precedence[Token::TOK_MULT] = 100;
        precedence[Token::TOK_DIV] = 100;
        precedence[Token::TOK_MODULUS] = 100;

        precedence[Token::TOK_PLUS] = 80;
        precedence[Token::TOK_MINUS] = 80;

        precedence[Token::TOK_LSHIFT] = 60;
        precedence[Token::TOK_RSHIFT] = 60;

        precedence[Token::TOK_LT] = 40;
        precedence[Token::TOK_LTE] = 40;
        precedence[Token::TOK_GT] = 40;
        precedence[Token::TOK_GTE] = 40;

        precedence[Token::TOK_EQEQUAL] = 20;
        precedence[Token::TOK_NEQUAL] = 20;

        precedence[Token::TOK_BITAND] = 10;

        precedence[Token::TOK_BITXOR] = 7;

        precedence[Token::TOK_BITOR] = 5;

        precedence[Token::TOK_LOGAND] = 5;
        precedence[Token::TOK_LOGOR] = 5;
    }

    Token::TokType check_next() const {
        if (curr_pos >= tok_len)
            return Token::TOK_EOF;

        if ((curr_pos + 1) >= tok_len)
            return Token::TOK_EOF;

        return tok_list[curr_pos + 1].tok_ty;
    }

    void advance() { curr_pos += 1; }

};

#endif // __PARSER_H__
