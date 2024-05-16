#include "../inc/parser.h"
#include <memory>

std::unique_ptr<ExpNode> Parser::parse_expr() {
    return nullptr;
}

std::pair<std::unique_ptr<Type>, std::string> Parser::parse_arg() {
    return {};
}

// fdecl ::== fun <ident> ( arg1,...,argn) { <stmt1>; <stmt2>; ... }
std::unique_ptr<Decls> Parser::parse_fdecl() {
    expect(Token::TOK_FUNCTION, "`fun` keyword");

    advance();

    expect(Token::TOK_IDENT, "identifier for function name");

    advance();

    expect(Token::TOK_LPAREN, "`(`");

    advance();

    while (expect(Token::TOK_RPAREN, "`)` for ending function arg list")) {
        switch (tok_list[curr_pos].tok_ty) {
            case Token::TOK_TYPE_INT:
            case Token::TOK_TYPE_STRING:
            case Token::TOK_TYPE_BOOL:
                advance();
                return parse_gvdecl();
                break;

            default:
                std::cout << "[ERR]: Invalid syntax at: " <<
                    "[" << tok_list[curr_pos].line_num << "," <<
                    tok_list[curr_pos].line_num << "]\n";
                return nullptr;
        }
        advance();
    }

    return nullptr;
}

// vdecl ::== <type> : <ident> = <exp>
std::unique_ptr<Decls> Parser::parse_gvdecl() {
    // TODO: Parse pointers
    // TODO: Maintain an environment to map variables to their types
    if (check_next() == Token::TOK_DEREF) {
        while (check_next() == Token::TOK_DEREF)
            advance();
    }

    expect(Token::TOK_COLON, "identifier");

    advance();

    expect(Token::TOK_COLON, "`:` operator");

    std::string& gvname = tok_list[curr_pos].lexeme;

    advance();

    std::unique_ptr<ExpNode> init_exp = parse_expr();
    return std::make_unique<GlobalDecl>(gvname, std::move(init_exp));
}

std::unique_ptr<Decls> Parser::parse_decl() {
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_TYPE_INT:
        case Token::TOK_TYPE_STRING:
        case Token::TOK_TYPE_BOOL:
            advance();
            return parse_gvdecl();
            break;

        case Token::TOK_FUNCTION:
            return parse_fdecl();
            break;

        default:
            std::cout << "[ERR]: Invalid syntax at: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].line_num << "]\n";
            return nullptr;
            break;
    }
}

void Parser::parse_prog() {
    while (curr_pos < tok_len)
        prog.decl_list.push_back(parse_decl());
}
