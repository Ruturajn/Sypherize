#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <iostream>

class Token {
public:
    enum TokType {
        TOK_LBRACE, // "{"
        TOK_RBRACE, // "}"

        TOK_LPAREN, // "("
        TOK_RPAREN, // ")"

        TOK_LBRACKET,   // "["
        TOK_RBRACKET,   // "]"

        TOK_COMMA,  // ","
        TOK_DOT,    // "."
        TOK_SEMIC,  // ";"
        TOK_COLON,  // ":"
        TOK_BTICK,  // "`"
        TOK_FUNC_RET,   // "->"

        TOK_PLUS,   // "+"
        TOK_MINUS,  // "-"
        TOK_MULT,   // "*"
        TOK_DIV,    // "/"
        TOK_LSHIFT, // "<<"
        TOK_RSHIFT, // ">>"
        TOK_MODULUS,    // "%"

        TOK_BITAND, // "&"
        TOK_BITOR,  // "|"
        TOK_BITXOR, // "^"
        TOK_NEG,    // "~"
        TOK_NOT,    // "!"
        TOK_EQUAL,  // "="

        TOK_EQEQUAL,  // "=="
        TOK_NEQUAL, // "!="
        TOK_GT,     // ">"
        TOK_LT,     // "<"
        TOK_GTE,    // ">="
        TOK_LTE,    // "<="
        TOK_LOGAND, // "&&"
        TOK_LOGOR,  // "||"

        TOK_DEREF, // "@"
        TOK_ADDROF, // "#"

        TOK_ONELINE_COMMENT,    // "$"
        TOK_MULTILINE_COMMENT_BEG,  // "(^"
        TOK_MULTILINE_COMMENT_END,  // "^)"

        TOK_NUMBER, // Literal number (Positive or negative int)
        TOK_STRING, // Literal string
        TOK_IDENT,  // Identifier

        TOK_TYPE_INT,   // Keyword: "int"
        TOK_TYPE_STRING,    // Keyword: "string"
        TOK_TYPE_BOOL,  // Keyword: "bool"

        TOK_IF, // Keyword: "if"
        TOK_ELSE,   // Keyword: "else"
        TOK_FOR,    // Keyword: "for"
        TOK_WHILE,  // Keyword: "while"
        TOK_RETURN, // Keyword: "return"
        TOK_FUNCTION,   // Keyword: "fun"

        TOK_EOF
    };

    enum TokType tok_ty;
    ssize_t col_num;
    ssize_t line_num;
    std::string lexeme;

    Token(enum TokType _tok_ty, ssize_t _col_num, ssize_t _line_num,
          std::string _lexeme) :
        tok_ty(_tok_ty), col_num(_col_num), line_num(_line_num),
            lexeme (std::move(_lexeme)) {}

    void print_token() const {
        switch (tok_ty) {
            case TOK_LBRACE:
                std::cout << "TOK_LBRACE";
                break;

            case TOK_RBRACE:
                std::cout << "TOK_RBRACE";
                break;

            case TOK_LPAREN:
                std::cout << "TOK_LPAREN";
                break;

            case TOK_RPAREN:
                std::cout << "TOK_RPAREN";
                break;

            case TOK_LBRACKET:
                std::cout << "TOK_LBRACKET";
                break;

            case TOK_RBRACKET:
                std::cout << "TOK_LBRACKET";
                break;

            case TOK_COMMA:
                std::cout << "TOK_COMMA";
                break;

            case TOK_DOT:
                std::cout << "TOK_DOT";
                break;

            case TOK_SEMIC:
                std::cout << "TOK_SEMIC";
                break;

            case TOK_COLON:
                std::cout << "TOK_COLON";
                break;

            case TOK_BTICK:
                std::cout << "TOK_BTICK";
                break;

            case TOK_FUNC_RET:
                std::cout << "TOK_FUNC_RET";
                break;

            case TOK_PLUS:
                std::cout << "TOK_PLUS";
                break;

            case TOK_MINUS:
                std::cout << "TOK_MINUS";
                break;

            case TOK_MULT:
                std::cout << "TOK_MULT";
                break;

            case TOK_DIV:
                std::cout << "TOK_DIV";
                break;

            case TOK_LSHIFT:
                std::cout << "TOK_LSHIFT";
                break;

            case TOK_RSHIFT:
                std::cout << "TOK_RSHIFT";
                break;

            case TOK_MODULUS:
                std::cout << "TOK_MODULUS";
                break;

            case TOK_BITAND:
                std::cout << "TOK_BITAND";
                break;

            case TOK_BITOR:
                std::cout << "TOK_BITOR";
                break;

            case TOK_BITXOR:
                std::cout << "TOK_BITXOR";
                break;

            case TOK_NEG:
                std::cout << "TOK_NEG";
                break;

            case TOK_NOT:
                std::cout << "TOK_NOT";
                break;

            case TOK_EQUAL:
                std::cout << "TOK_EQUAL";
                break;

            case TOK_EQEQUAL:
                std::cout << "TOK_EQEQUAL";
                break;

            case TOK_NEQUAL:
                std::cout << "TOK_NEQUAL";
                break;

            case TOK_GT:
                std::cout << "TOK_GT";
                break;

            case TOK_LT:
                std::cout << "TOK_LT";
                break;

            case TOK_GTE:
                std::cout << "TOK_GTE";
                break;

            case TOK_LTE:
                std::cout << "TOK_LTE";
                break;

            case TOK_LOGAND:
                std::cout << "TOK_LOGAND";
                break;

            case TOK_LOGOR:
                std::cout << "TOK_LOGOR";
                break;

            case TOK_DEREF:
                std::cout << "TOK_DEREF";
                break;

            case TOK_ADDROF:
                std::cout << "TOK_ADDROF";
                break;

            case TOK_ONELINE_COMMENT:
                std::cout << "TOK_ONELINE_COMMENT";
                break;

            case TOK_MULTILINE_COMMENT_BEG:
                std::cout << "TOK_MULTILINE_COMMENT_BEG";
                break;

            case TOK_MULTILINE_COMMENT_END:
                std::cout << "TOK_MULTILINE_COMMENT_END";
                break;

            case TOK_NUMBER:
                std::cout << "TOK_NUMBER: " << lexeme;
                break;

            case TOK_STRING:
                std::cout << "TOK_STRING: " << lexeme;
                break;

            case TOK_IDENT:
                std::cout << "TOK_IDENT: " << lexeme;
                break;

            case TOK_TYPE_INT:
                std::cout << "TOK_TYPE_INT";
                break;

            case TOK_TYPE_STRING:
                std::cout << "TOK_TYPE_STRING";
                break;

            case TOK_TYPE_BOOL:
                std::cout << "TOK_TYPE_BOOL";
                break;

            case TOK_IF:
                std::cout << "TOK_IF";
                break;

            case TOK_ELSE:
                std::cout << "TOK_ELSE";
                break;

            case TOK_FOR:
                std::cout << "TOK_FOR";
                break;

            case TOK_WHILE:
                std::cout << "TOK_WHILE";
                break;

            case TOK_RETURN:
                std::cout << "TOK_RETURN";
                break;

            case TOK_FUNCTION:
                std::cout << "TOK_FUNCTION";
                break;

            case TOK_EOF:
                std::cout << "TOK_EOF";
                break;
        }
        std::cout << "<" << line_num << "," << col_num << ">";
    }
};

#endif // __TOKEN_H__
