#include "../inc/parser.h"
#include <memory>

const std::string Parser::global_key = "__global__";

std::unique_ptr<ExpNode> Parser::parse_binop(int prev_prec,
                                             std::unique_ptr<ExpNode> lhs) {
    Token curr_tok = tok_list[curr_pos];

    while (prev_prec < get_precedence(curr_tok.tok_ty)) {
        advance();

        if (curr_pos >= tok_len)
            return lhs;

        auto rhs = parse_expr(get_precedence(curr_tok.tok_ty));

        auto binop = std::make_unique<BinopExpNode>(conv_binop(curr_tok),
                        std::move(lhs), std::move(rhs));

        lhs = std::move(binop);

        curr_tok = tok_list[curr_pos];
    }

    return lhs;
}

std::unique_ptr<ExpNode> Parser::parse_expr(int prev_prec) {
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_LPAREN: {
            advance();

            // Ignore previous precedence
            auto ret = parse_expr(0);

            advance();

            expect(Token::TOK_RPAREN, "`)` closing paren");

            return ret;
            break;
        }

        case Token::TOK_NUMBER:
            if (is_next_binop()) {
                std::unique_ptr<ExpNode> lhs =
                    std::make_unique<NumberExpNode>(stol(tok_list[curr_pos].lexeme));

                advance();

                return parse_binop(prev_prec, std::move(lhs));
            }
            else
                return std::make_unique<NumberExpNode>(stol(tok_list[curr_pos].lexeme));
            break;

        case Token::TOK_STRING:
            return std::make_unique<StringExpNode>(tok_list[curr_pos].lexeme);
            break;

        case Token::TOK_IDENT: {
            auto id = std::make_unique<IdExpNode>(tok_list[curr_pos].lexeme);

            Token::TokType next_tok = check_next();

            // Parse function calls
            if (next_tok == Token::TOK_LPAREN) {
                std::string& fun_name = tok_list[curr_pos].lexeme;

                // Consume TOK_LPAREN
                advance();

                advance();

                std::vector<ExpNode*> f_args {};

                while ((curr_pos < tok_len) &&
                        (tok_list[curr_pos].tok_ty != Token::TOK_RPAREN)) {
                    f_args.push_back(parse_expr(prev_prec).release());
                    advance();

                    if (tok_list[curr_pos].tok_ty == Token::TOK_RPAREN)
                        break;

                    expect(Token::TOK_COMMA, "`,` operator to separate function"
                            " arguments");
                    advance();
                }

                auto funcall = std::make_unique<FunCallExpNode>(fun_name, f_args);

                if (tok_list[curr_pos].tok_ty == Token::TOK_LBRACKET) {
                    auto index = parse_expr(prev_prec);

                    advance();
                    expect(Token::TOK_RBRACKET, "`]` for index operation");

                    return std::make_unique<IndexExpNode>(std::move(funcall),
                            std::move(index));
                }

                return funcall;
            }

            // Index expression
            if (next_tok == Token::TOK_LBRACKET) {

                // Consume the TOK_LBRACKET
                advance();

                advance();

                auto index = parse_expr(prev_prec);

                advance();
                expect(Token::TOK_RBRACKET, "`]` for index operation");

                return std::make_unique<IndexExpNode>(std::move(id),
                        std::move(index));
            }

            if (is_next_binop()) {
                advance();

                return parse_binop(prev_prec, std::move(id));
            }

            return id;
            break;
        }

        case Token::TOK_BOOL_TRUE:
            return std::make_unique<BoolExpNode>(true);
            break;

        case Token::TOK_BOOL_FALSE:
            return std::make_unique<BoolExpNode>(false);
            break;

        case Token::TOK_PLUS:
        case Token::TOK_MINUS:
        case Token::TOK_MULT:
        case Token::TOK_DIV:
        case Token::TOK_LSHIFT:
        case Token::TOK_RSHIFT:
        case Token::TOK_MODULUS:
        case Token::TOK_BITAND:
        case Token::TOK_BITOR:
        case Token::TOK_BITXOR:
        case Token::TOK_EQEQUAL:
        case Token::TOK_NEQUAL:
        case Token::TOK_GT:
        case Token::TOK_LT:
        case Token::TOK_GTE:
        case Token::TOK_LTE:
        case Token::TOK_LOGAND:
        case Token::TOK_LOGOR:
            std::cout << "[ERR]: Invalid syntax at parse_expr, found binary"
                " op without preceding compatible type expr: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].col_num << "]\n";
            return nullptr;
            break;

        case Token::TOK_NEG:
        case Token::TOK_NOT:
        case Token::TOK_DEREF:
        case Token::TOK_ADDROF: {
            auto unop = conv_unop(tok_list[curr_pos]);
            advance();
            return std::make_unique<UnopExpNode>(unop, parse_expr(prev_prec));
        }

        default:
            std::cout << "[ERR]: Invalid syntax at parse_expr: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].col_num << "]\n";
            return nullptr;
    }
}

std::pair<Type*, std::string> Parser::parse_arg(const std::string& fn_name) {

    auto p_type = parse_type();

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& p_name = tok_list[curr_pos].lexeme;

    advance();

    // Bind variable name in global context to it's type.
    var_bind_ctxt(fn_name, p_name, p_type);

    if (tok_list[curr_pos].tok_ty != Token::TOK_RPAREN) {
        expect(Token::TOK_COMMA, "`,` to delimit function paramaters");
        advance();
    }

    return {p_type, p_name};
}


std::unique_ptr<ExpNode> Parser::parse_new_type() {
    enum DeclType type_set = conv_type(tok_list[curr_pos].tok_ty);

    int indirection_count = 0;
    while (check_next() == Token::TOK_DEREF) {
        advance();
        indirection_count += 1;
    }

    auto p_type = build_type(indirection_count, type_set);
    std::unique_ptr<Type> p_type_uptr(p_type);

    if (check_next() == Token::TOK_LBRACKET) {

        // Consume TOK_LBRACKET
        advance();

        advance();

        auto size = parse_expr(0);
        advance();

        expect(Token::TOK_RBRACKET, "`]` for ending array decl");

        return std::make_unique<NewExpNode>(std::move(p_type_uptr), std::move(size));
    }

    return std::make_unique<NewExpNode>(std::move(p_type_uptr), nullptr);
}

StmtNode* Parser::parse_vdecl(const std::string& fname) {
    auto vtype = parse_type();

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& vname = tok_list[curr_pos].lexeme;

    advance();

    // Bind variable name in local function context to it's type.
    if (env.find(fname) != env.end()) {
        if (env[fname].find(vname) != env[fname].end())
                std::cout << "[ERR]: Redefinition of variable at: "
                    << "[" << tok_list[curr_pos].line_num << "," <<
                    tok_list[curr_pos].col_num << "]\n";
    }
    var_bind_ctxt(fname, vname, vtype);

    if (tok_list[curr_pos].tok_ty == Token::TOK_LBRACE) {

        advance();

        std::vector<ExpNode*> init_exps {};

        while ((curr_pos < tok_len) &&
                (tok_list[curr_pos].tok_ty != Token::TOK_RBRACE)) {
            init_exps.push_back(parse_expr(0).release());
            advance();

            if (tok_list[curr_pos].tok_ty == Token::TOK_RBRACE)
                break;

            expect(Token::TOK_COMMA, "`,` operator to separate function"
                    " arguments");
            advance();
        }

        advance();

        std::unique_ptr<Type> vtype_ptr(vtype);
        auto carr_exp = std::make_unique<CArrExpNode>(std::move(vtype_ptr), init_exps);

        return new DeclStmtNode(vname, std::move(carr_exp));
    }

    expect(Token::TOK_EQUAL, "`=` operator");
    advance();

    if (tok_list[curr_pos].tok_ty == Token::TOK_NEW) {
        advance();

        auto new_exp = parse_new_type();
        advance();
        return new DeclStmtNode(vname, std::move(new_exp));
    }

    std::unique_ptr<ExpNode> init_exp = parse_expr(0);
    advance();

    return new DeclStmtNode(vname, std::move(init_exp));
}

StmtNode* Parser::parse_stmt(const std::string& fname) {
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_TYPE_INT:
        case Token::TOK_TYPE_STRING:
        case Token::TOK_TYPE_BOOL:
            return parse_vdecl(fname);

        case Token::TOK_RETURN:
            advance();

            if (tok_list[curr_pos].tok_ty == Token::TOK_SEMIC) {
                advance();
                return new RetStmtNode();
            }
            else{
                auto ret_node = new RetStmtNode(parse_expr(0));
                advance();
                return ret_node;
            }
            break;

        default:
            return nullptr;
    }
}

std::vector<StmtNode*> Parser::parse_block(const std::string& fname) {
    std::vector<StmtNode*> stmts {};
    while (curr_pos < tok_len && tok_list[curr_pos].tok_ty != Token::TOK_RBRACE) {
        stmts.push_back(parse_stmt(fname));
        expect(Token::TOK_SEMIC, "terminating `;` operator");
        advance();
    }
    return stmts;
}

// fdecl ::== fun <ident> (arg1,...,argn) { <stmt1>; <stmt2>; ... }
Decls* Parser::parse_fdecl() {
    expect(Token::TOK_FUNCTION, "`fun` keyword");

    advance();

    expect(Token::TOK_IDENT, "identifier for function name");
    std::string& f_name = tok_list[curr_pos].lexeme;

    advance();

    expect(Token::TOK_LPAREN, "`(`");
    advance();

    std::vector<std::pair<Type*, std::string>> params {};

    while ((curr_pos < tok_len) &&
            (tok_list[curr_pos].tok_ty != Token::TOK_RPAREN)) {
        switch (tok_list[curr_pos].tok_ty) {
            case Token::TOK_TYPE_INT:
            case Token::TOK_TYPE_STRING:
            case Token::TOK_TYPE_BOOL:
                params.push_back(parse_arg(f_name));
                break;

            default:
                std::cout << "[ERR]: Invalid syntax for function parameters: "
                    << "[" << tok_list[curr_pos].line_num << "," <<
                    tok_list[curr_pos].col_num << "]\n";
                return nullptr;
        }
    }

    advance();

    expect(Token::TOK_FUNC_RET, "`->` for function return type");
    advance();

    auto fret_type = parse_type();

    advance();

    expect(Token::TOK_LBRACE, "`{` for function definition");
    advance();

    std::vector<StmtNode*> fn_body = parse_block(f_name);

    expect(Token::TOK_RBRACE, "`}` for ending function definition");
    advance();

    std::unique_ptr<Type> fret_type_uptr(fret_type);
    return new FunDecl(std::move(fret_type_uptr), f_name, params, fn_body);
}

// vdecl ::== <type> : <ident> = <exp>
Decls* Parser::parse_gvdecl() {
    auto gvtype = parse_type();

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& gvname = tok_list[curr_pos].lexeme;

    advance();

    // Bind variable name in global context to it's type.
    var_bind_ctxt(global_key, gvname, gvtype);

    expect(Token::TOK_EQUAL, "`=` operator");
    advance();

    std::unique_ptr<ExpNode> init_exp = parse_expr(0);
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
                tok_list[curr_pos].col_num << "]\n";
            return nullptr;
            break;
    }
}

void Parser::parse_prog() {
    while (curr_pos < tok_len)
        prog.decl_list.push_back(parse_decl());
}
