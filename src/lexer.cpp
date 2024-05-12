#include "../inc/lexer.h"

char Lexer::check_next() {
    if ((curr_pos >= data_sz) || ((curr_pos + 1) >= data_sz))
        return 0;

    return file_data[curr_pos + 1];
}

bool Lexer::skip_until_newline() {
    while ((file_data[curr_pos] != '\n') && (curr_pos <= data_sz)) {
        curr_pos += 1;
    }
    if (curr_pos >= data_sz)
        return false;

    return file_data[curr_pos] == '\n';
}

bool Lexer::skip_until_comment_close() {
    while ((file_data[curr_pos] != '^') &&
            (curr_pos <= data_sz) &&
            ((curr_pos + 1) <= data_sz) &&
            file_data[curr_pos + 1] != ')') {
        curr_pos += 1;
    }

    if (curr_pos >= data_sz)
        return false;

    return (file_data[curr_pos] == ')' && file_data[curr_pos - 1] == '^');
}

Token Lexer::create_token(ssize_t tok_sz, enum Token::TokType t_ty) {
    std::string lexeme = file_data.substr(curr_pos, tok_sz);
    return Token(t_ty, col_num, l_num, lexeme);
}

void Lexer::push_tok(ssize_t tok_sz, enum Token::TokType t_ty) {
    Token tok = create_token(tok_sz, t_ty);
    tok_list.push_back(tok);
    curr_pos += tok_sz;
    col_num += tok_sz;
}

void Lexer::lex() {
    size_t data_len = file_data.size();
    char curr_c = 0;
    while (curr_pos <= data_len) {
        curr_c = file_data[curr_pos];
        switch (curr_c) {
            case '{':
                push_tok(1, Token::TOK_LBRACE);
                break;

            case '}':
                push_tok(1, Token::TOK_RBRACE);
                break;

            case '(':
                if (check_next() == '^')
                    skip_until_comment_close();
                else
                    push_tok(1, Token::TOK_LPAREN);
                break;

            case ')':
                push_tok(1, Token::TOK_RPAREN);
                break;

            case '[':
                push_tok(1, Token::TOK_LBRACKET);
                break;

            case ']':
                push_tok(1, Token::TOK_RBRACKET);
                break;

            case ',':
                push_tok(1, Token::TOK_COMMA);
                break;

            case '.':
                push_tok(1, Token::TOK_DOT);
                break;

            case ';':
                push_tok(1, Token::TOK_SEMIC);
                break;

            case ':':
                push_tok(1, Token::TOK_COLON);
                break;

            case '`':
                push_tok(1, Token::TOK_BTICK);
                break;

            case '+':
                push_tok(1, Token::TOK_PLUS);
                break;

            case '-':
                if (check_next() == '>')
                    push_tok(2, Token::TOK_FUNC_RET);
                else
                    push_tok(1, Token::TOK_MINUS);
                break;

            case '*':
                push_tok(1, Token::TOK_MULT);
                break;

            case '/':
                push_tok(1, Token::TOK_DIV);
                break;

            case '<': {
                char next_char = check_next();
                if (next_char == '<')
                    push_tok(2, Token::TOK_LSHIFT);
                else if (next_char == '=')
                    push_tok(2, Token::TOK_LTE);
                else
                    push_tok(1, Token::TOK_LT);
                break;
            }

            case '>': {
                char next_char = check_next();
                if (next_char == '>')
                    push_tok(2, Token::TOK_RSHIFT);
                else if (next_char == '=')
                    push_tok(2, Token::TOK_GTE);
                else
                    push_tok(1, Token::TOK_GT);
                break;
            }

            case '%':
                push_tok(1, Token::TOK_MODULUS);
                break;

            case '&':
                push_tok(1, Token::TOK_BITAND);
                break;

            case '|':
                push_tok(1, Token::TOK_BITOR);
                break;

            case '^':
                push_tok(1, Token::TOK_BITXOR);
                break;

            case '~':
                push_tok(1, Token::TOK_NEG);
                break;

            case '!':
                if (check_next() == '=')
                    push_tok(2, Token::TOK_NEQUAL);
                else
                    push_tok(1, Token::TOK_NOT);
                break;

            case '=':
                if (check_next() == '=')
                    push_tok(2, Token::TOK_EQEQUAL);
                else
                    push_tok(2, Token::TOK_EQUAL);
                break;

            case '@':
                push_tok(1, Token::TOK_DEREF);
                break;

            case '#':
                push_tok(1, Token::TOK_ADDROF);
                break;

            case '$':
                skip_until_newline();
                break;

            case '\n':
                l_num += 1;
                col_num = 0;
                break;

            case ' ':
            case '\r':
                curr_pos += 1;
                break;

            default:
                break;
        }
    }
}

void Lexer::print_tokens() {
    std::cout << "[\n";
    for (auto &t: tok_list) {
        t.print_token();
        std::cout << ",\n";
    }
    std::cout << "]\n";
}
