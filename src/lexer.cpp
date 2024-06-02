#include "../inc/lexer.h"

char Lexer::check_next() {
    if ((curr_pos >= data_sz) || ((curr_pos + 1) >= data_sz))
        return 0;

    return file_data[curr_pos + 1];
}

bool Lexer::skip_until_newline() {
    while ((file_data[curr_pos] != '\n') && (curr_pos < data_sz)) {
        curr_pos += 1;
    }
    if (curr_pos >= data_sz)
        return false;

    curr_pos += 1;
    col_num = 1;
    l_num += 1;
    return file_data[curr_pos - 1] == '\n';
}

bool Lexer::skip_until_comment_close() {
    // Skip the comment beginning
    curr_pos += 2;
    bool found_hat = false;

    while (curr_pos < data_sz) {
        if (file_data[curr_pos] == '^')
            found_hat = true;

        if (file_data[curr_pos] == ')') {
            if (found_hat)
                break;
        }

        if (file_data[curr_pos] == '\n') {
            l_num += 1;
            col_num = 1;
        }

        if (file_data[curr_pos] == '$') {
            skip_until_newline();
            continue;
        }

        if (file_data[curr_pos] == '(' &&
            ((curr_pos + 1) < data_sz) &&
            file_data[curr_pos + 1] == '^') {
            skip_until_comment_close();
            continue;
        }

        curr_pos += 1;
        col_num += 1;
    }

    if (curr_pos >= data_sz)
        return false;

    curr_pos += 1;
    col_num += 1;
    return (file_data[curr_pos - 1] == ')' && found_hat);
}

Token Lexer::create_token(ssize_t tok_sz, enum Token::TokType t_ty) {
    std::string lexeme = file_data.substr(curr_pos, tok_sz);

    if (lexeme == "int")
        return Token(Token::TOK_TYPE_INT, col_num, l_num, lexeme);

    else if (lexeme == "string")
        return Token(Token::TOK_TYPE_STRING, col_num, l_num, lexeme);

    else if (lexeme == "bool")
        return Token(Token::TOK_TYPE_BOOL, col_num, l_num, lexeme);

    else if (lexeme == "void")
        return Token(Token::TOK_TYPE_VOID, col_num, l_num, lexeme);

    else if (lexeme == "true")
        return Token(Token::TOK_BOOL_TRUE, col_num, l_num, lexeme);

    else if (lexeme == "false")
        return Token(Token::TOK_BOOL_FALSE, col_num, l_num, lexeme);

    else if (lexeme == "if")
        return Token(Token::TOK_IF, col_num, l_num, lexeme);

    else if (lexeme == "else")
        return Token(Token::TOK_ELSE, col_num, l_num, lexeme);

    else if (lexeme == "for")
        return Token(Token::TOK_FOR, col_num, l_num, lexeme);

    else if (lexeme == "while")
        return Token(Token::TOK_WHILE, col_num, l_num, lexeme);

    else if (lexeme == "return")
        return Token(Token::TOK_RETURN, col_num, l_num, lexeme);

    else if (lexeme == "fun")
        return Token(Token::TOK_FUNCTION, col_num, l_num, lexeme);

    else if (lexeme == "new")
        return Token(Token::TOK_NEW, col_num, l_num, lexeme);

    else if (lexeme == "null")
        return Token(Token::TOK_NULL, col_num, l_num, lexeme);

    else if (lexeme == "__global__") {
        if (failed == false)
            failed = true;
        std::cerr << "Disallowed/Reserved variable name/function name"
            " `__global__` used at: [" << l_num <<
            "," << col_num << "]\n";
        return Token(t_ty, col_num, l_num, lexeme);
    }

    else
        return Token(t_ty, col_num, l_num, lexeme);
}

void Lexer::push_tok(ssize_t tok_sz, enum Token::TokType t_ty) {
    Token tok = create_token(tok_sz, t_ty);
    tok_list.push_back(tok);
    curr_pos += tok_sz;
    col_num += tok_sz;
}

void Lexer::lex_number() {
    ssize_t final_pos = curr_pos;
    final_pos += 1;
    while (final_pos < data_sz) {
        if (!is_num(file_data[final_pos]))
            break;
        final_pos += 1;
    }
    push_tok((final_pos - curr_pos), Token::TOK_NUMBER);
}

void Lexer::lex_identifier() {
    ssize_t final_pos = curr_pos;
    while (final_pos < data_sz) {
        if ((!is_alpha(file_data[final_pos])) &&
                (!is_num(file_data[final_pos])))
            break;
        final_pos += 1;
    }
    push_tok((final_pos - curr_pos), Token::TOK_IDENT);
}

void Lexer::lex_string() {
    ssize_t final_pos = curr_pos;
    final_pos += 1;
    while (final_pos < data_sz) {
        if (file_data[final_pos] == '"') {
            final_pos += 1;
            break;
        }
        final_pos += 1;
    }
    push_tok((final_pos - curr_pos), Token::TOK_STRING);
}

void Lexer::lex() {
    char curr_c = 0;
    while (curr_pos < data_sz) {
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

            case '+': {
                char next_char = check_next();
                if (is_num(next_char)) {
                    curr_pos += 1;
                    lex_number();
                }
                else
                    push_tok(1, Token::TOK_PLUS);
                break;
            }

            case '-': {
                char next_char = check_next();
                if (next_char == '>')
                    push_tok(2, Token::TOK_FUNC_RET);
                else if (is_num(next_char))
                    lex_number();
                else
                    push_tok(1, Token::TOK_MINUS);
                break;
            }

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
                if (check_next() == '&')
                    push_tok(2, Token::TOK_LOGAND);
                else
                    push_tok(1, Token::TOK_BITAND);
                break;

            case '|':
                if (check_next() == '|')
                    push_tok(2, Token::TOK_LOGOR);
                else
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
                    push_tok(1, Token::TOK_EQUAL);
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
                col_num = 1;
                curr_pos += 1;
                break;

            case '"':
                lex_string();
                break;

            case ' ':
            case '\r':
                curr_pos += 1;
                col_num += 1;
                break;

            default:
                if (is_alpha(curr_c))
                    lex_identifier();
                else if (is_num(curr_c))
                    lex_number();
                else {
                    if (failed == false)
                        failed = true;
                    std::cerr << "Invalid character found at: [" << l_num <<
                        "," << col_num << "]\n";
                    curr_pos += 1;
                }
                break;
        }
    }
}

void Lexer::print_tokens() const {
    std::cout << "[\n";
    for (auto &t: tok_list) {
        t.print_token();
        std::cout << ",\n";
    }
    std::cout << "]\n";
}
