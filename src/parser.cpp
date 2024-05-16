#include "../inc/parser.h"
#include <memory>

const std::string Parser::global_key = "__global__";

std::unique_ptr<ExpNode> Parser::parse_expr() {
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_NUMBER:
            return std::make_unique<NumberExpNode>(stol(tok_list[curr_pos].lexeme));
            break;
        case Token::TOK_STRING:
            return std::make_unique<StringExpNode>(tok_list[curr_pos].lexeme);
            break;
        default:
            std::cout << "[ERR]: Invalid syntax at parse_expr: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].line_num << "]\n";
            return nullptr;
    }
}

std::pair<std::unique_ptr<Type>, std::string> Parser::parse_arg() {
    return {};
}

// fdecl ::== fun <ident> (arg1,...,argn) { <stmt1>; <stmt2>; ... }
Decls* Parser::parse_fdecl() {
    expect(Token::TOK_FUNCTION, "`fun` keyword");

    advance();

    expect(Token::TOK_IDENT, "identifier for function name");

    advance();

    expect(Token::TOK_LPAREN, "`(`");

    advance();

    // TODO: parse commas, as argument separators.
    while (expect(Token::TOK_RPAREN, "`)` for ending function arg list")) {
        switch (tok_list[curr_pos].tok_ty) {
            case Token::TOK_TYPE_INT:
            case Token::TOK_TYPE_STRING:
            case Token::TOK_TYPE_BOOL:
                parse_arg();
                return nullptr;
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
Decls* Parser::parse_gvdecl() {
    enum DeclType type_set;

    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_TYPE_INT:
            type_set = DECL_TYPE_INT;
            break;
        case Token::TOK_TYPE_STRING:
            type_set = DECL_TYPE_STRING;
            break;
        case Token::TOK_TYPE_BOOL:
            type_set = DECL_TYPE_BOOL;
            break;
        default:
            std::cout << "[ERR]: Invalid syntax at: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].line_num << "]\n";
            return nullptr;
    }

    int indirection_count = 0;
    while (check_next() == Token::TOK_DEREF) {
        advance();
        indirection_count += 1;
    }

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& gvname = tok_list[curr_pos].lexeme;

    advance();

    // Bind variable name in global context to it's type.
    var_bind_ctxt(global_key, gvname,
                  parse_type(indirection_count, type_set));

    expect(Token::TOK_EQUAL, "`=` operator");
    advance();

    std::unique_ptr<ExpNode> init_exp = parse_expr();
    advance();

    expect(Token::TOK_SEMIC, "terminating `;` operator");
    advance();

    return new GlobalDecl(gvname, std::move(init_exp));
}

Decls* Parser::parse_decl() {
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_TYPE_INT:
        case Token::TOK_TYPE_STRING:
        case Token::TOK_TYPE_BOOL:
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
