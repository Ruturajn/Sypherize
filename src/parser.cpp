#include "../inc/parser.h"
#include <memory>

///===-------------------------------------------------------------------===///
/// Helper Functions
///===-------------------------------------------------------------------===///

Parser::Parser(std::vector<Token>& _tok_list, Diagnostics* _diag)
    : tok_list(_tok_list), curr_pos(0),
        tok_len(_tok_list.size()), precedence({}), prog(Program()),
        failed(false), diag(_diag) {

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

    precedence[Token::TOK_LOGOR] = 3;
}

enum Parser::DeclType Parser::conv_type(enum Token::TokType t_ty) {
    switch (t_ty) {
        case Token::TOK_TYPE_INT:
            return DECL_TYPE_INT;

        case Token::TOK_TYPE_STRING:
            return DECL_TYPE_STRING;

        case Token::TOK_TYPE_BOOL:
            return DECL_TYPE_BOOL;

        default:
            expect(Token::TOK_EOF, "Invalid syntax for decl type");
            return DECL_TYPE_INT;
    }
}

SRange Parser::make_srange(ssize_t beg_pos) {
    if ((beg_pos >= 0) && (curr_pos < tok_len) && (curr_pos >= 0)) {
        return SRange(SLoc(tok_list[beg_pos].line_num,
                           tok_list[beg_pos].col_num),
                      SLoc(tok_list[curr_pos].line_num,
                           tok_list[curr_pos].col_num));

    } else if (curr_pos >= tok_len) {
        return SRange(SLoc(tok_list[beg_pos].line_num,
                           tok_list[beg_pos].col_num),
                      SLoc(tok_list[curr_pos - 1].line_num,
                           tok_list[curr_pos - 1].col_num));
    }
    else
        return SRange(SLoc(0, 0), SLoc(0,0));
}

Token::TokType Parser::check_next() const {
    if (curr_pos >= tok_len)
        return Token::TOK_EOF;

    if ((curr_pos + 1) >= tok_len)
        return Token::TOK_EOF;

    return tok_list[curr_pos + 1].tok_ty;
}

bool Parser::is_next_binop() const {
    Token::TokType next_tok = check_next();
    switch (next_tok) {
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
            return true;
        default:
            return false;
    }
}

int Parser::get_precedence(enum Token::TokType t_ty) {
    if (precedence.find(t_ty) != precedence.end())
        return precedence[t_ty];

    return -1;
}

BinopType Parser::conv_binop(const Token& t) {
    switch (t.tok_ty) {
        case Token::TOK_PLUS:
            return BinopType::BINOP_PLUS;

        case Token::TOK_MINUS:
            return BinopType::BINOP_MINUS;

        case Token::TOK_MULT:
            return BinopType::BINOP_MULT;

        case Token::TOK_DIV:
            return BinopType::BINOP_DIVIDE;

        case Token::TOK_LSHIFT:
            return BinopType::BINOP_LSHIFT;

        case Token::TOK_RSHIFT:
            return BinopType::BINOP_RSHIFT;

        case Token::TOK_MODULUS:
            return BinopType::BINOP_MODULUS;

        case Token::TOK_BITAND:
            return BinopType::BINOP_BITAND;

        case Token::TOK_BITOR:
            return BinopType::BINOP_BITOR;

        case Token::TOK_BITXOR:
            return BinopType::BINOP_BITXOR;

        case Token::TOK_EQEQUAL:
            return BinopType::BINOP_EQEQUAL;

        case Token::TOK_NEQUAL:
            return BinopType::BINOP_NEQUAL;

        case Token::TOK_GT:
            return BinopType::BINOP_GT;

        case Token::TOK_LT:
            return BinopType::BINOP_LT;

        case Token::TOK_GTE:
            return BinopType::BINOP_GTE;

        case Token::TOK_LTE:
            return BinopType::BINOP_LTE;

        case Token::TOK_LOGAND:
            return BinopType::BINOP_LOGAND;

        case Token::TOK_LOGOR:
            return BinopType::BINOP_LOGOR;

        default:
            expect(Token::TOK_EOF, "Invalid token for binop conversion", t);
            return BinopType::BINOP_PLUS;
    }
}

UnopType Parser::conv_unop(const Token& t) {
    switch (t.tok_ty) {
        case Token::TOK_NEG:
            return UnopType::UNOP_NEG;

        case Token::TOK_NOT:
            return UnopType::UNOP_NOT;

        case Token::TOK_DEREF:
            return UnopType::UNOP_DEREF;

        case Token::TOK_ADDROF:
            return UnopType::UNOP_ADDROF;

        default:
            expect(Token::TOK_EOF, "Invalid token for unop conversion", t);
            return UnopType::UNOP_NEG;
    }
}

void Parser::expect(Token::TokType t_ty, const char* error_str) {
    return expect(t_ty, error_str, tok_list[curr_pos]);
}

void Parser::expect(Token::TokType t_ty, const char* error_str, const Token& tok) {
    if (curr_pos >= tok_len)
        return;

    if (tok.tok_ty == t_ty)
        return;

    if (failed == false)
        failed = true;

    diag->print_error(tok.line_num, tok.col_num, error_str, tok.lexeme.size());
}

Type* Parser::build_type(int in_count, enum DeclType dt) {
    if (in_count == 0) {
        switch (dt) {
            case DECL_TYPE_INT:
                return new TInt();
                break;
            case DECL_TYPE_STRING:
                return new TString();
                break;
            case DECL_TYPE_BOOL:
                return new TBool();
                break;
        }
    }

    return new TRef(build_type(in_count - 1,dt));
}

Type* Parser::parse_type() {
    enum DeclType type_set = conv_type(tok_list[curr_pos].tok_ty);

    int indirection_count = 0;
    while (check_next() == Token::TOK_DEREF) {
        advance();
        indirection_count += 1;
    }

    if (check_next() == Token::TOK_LBRACKET) {
        advance();

        advance();
        expect(Token::TOK_RBRACKET, "`]` for ending array decl");

        return new TArray(build_type(indirection_count, type_set));
    }

    auto p_type = build_type(indirection_count, type_set);
    return p_type;
}

Type* Parser::parse_type_wo_arr() {
    enum DeclType type_set = conv_type(tok_list[curr_pos].tok_ty);

    int indirection_count = 0;
    while (check_next() == Token::TOK_DEREF) {
        advance();
        indirection_count += 1;
    }

    auto p_type = build_type(indirection_count, type_set);
    return p_type;
}

///===-------------------------------------------------------------------===///
/// Parsing AST Functions
///===-------------------------------------------------------------------===///
std::unique_ptr<ExpNode> Parser::parse_binop(int prev_prec,
                                             std::unique_ptr<ExpNode> lhs) {
    ssize_t beg_pos = curr_pos;

    Token curr_tok = tok_list[curr_pos];

    while (prev_prec < get_precedence(curr_tok.tok_ty)) {
        advance();

        if (curr_pos >= tok_len)
            return lhs;

        auto rhs = parse_expr(get_precedence(curr_tok.tok_ty));

        curr_pos += 1;
        auto srange = make_srange(beg_pos);
        curr_pos -= 1;

        auto b_srange = SRange(lhs->sr.beg, srange.end);

        auto binop = std::make_unique<BinopExpNode>(conv_binop(curr_tok),
                        std::move(lhs), std::move(rhs),
                        b_srange);

        lhs = std::move(binop);

        curr_tok = tok_list[curr_pos];
    }

    return lhs;
}

std::unique_ptr<ExpNode> Parser::parse_lhs(int prev_prec) {
    ssize_t beg_pos = curr_pos;
    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_IDENT: {
            auto id = std::make_unique<IdExpNode>(tok_list[curr_pos].lexeme,
                                                  make_srange(beg_pos));

            Token::TokType next_tok = check_next();

            // Index expression
            if (next_tok == Token::TOK_LBRACKET) {

                // Consume the TOK_LBRACKET
                advance();

                advance();

                auto index = parse_expr(prev_prec);

                advance();
                expect(Token::TOK_RBRACKET, "`]` for index operation");

                return std::make_unique<IndexExpNode>(std::move(id),
                        std::move(index), make_srange(beg_pos));
            }

            return id;
            break;
        }

        default:
            expect(Token::TOK_EOF, "Encountered invalid syntax");
            advance();
            return nullptr;
    }
}

std::unique_ptr<ExpNode> Parser::parse_expr(int prev_prec) {
    ssize_t beg_pos = curr_pos;

    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_LPAREN: {
            advance();

            // Ignore previous precedence
            auto ret = parse_expr(0);

            advance();

            expect(Token::TOK_RPAREN, "`)` closing paren");

            return ret;
        }

        case Token::TOK_NUMBER:
            if (is_next_binop()) {

                std::unique_ptr<ExpNode> lhs =
                    std::make_unique<NumberExpNode>(
                        stol(tok_list[curr_pos].lexeme),
                        make_srange(beg_pos)
                );

                advance();

                return parse_binop(prev_prec, std::move(lhs));
            }
            else
                return std::make_unique<NumberExpNode>(
                    stol(tok_list[curr_pos].lexeme),
                    make_srange(beg_pos)
                );

        case Token::TOK_STRING:
            return std::make_unique<StringExpNode>(
                tok_list[curr_pos].lexeme,
                make_srange(beg_pos)
            );

        case Token::TOK_IDENT: {
            auto id = std::make_unique<IdExpNode>(
                tok_list[curr_pos].lexeme,
                make_srange(beg_pos)
            );

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

                auto funcall = std::make_unique<FunCallExpNode>(
                    fun_name, f_args, make_srange(beg_pos));

                if (tok_list[curr_pos].tok_ty == Token::TOK_LBRACKET) {
                    auto index = parse_expr(prev_prec);

                    advance();
                    expect(Token::TOK_RBRACKET, "`]` for index operation");

                    return std::make_unique<IndexExpNode>(std::move(funcall),
                            std::move(index), make_srange(beg_pos));
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
                        std::move(index), make_srange(beg_pos));
            }

            if (is_next_binop()) {
                advance();

                return parse_binop(prev_prec, std::move(id));
            }

            return id;
        }

        case Token::TOK_BOOL_TRUE:
            return std::make_unique<BoolExpNode>(true, make_srange(beg_pos));
            break;

        case Token::TOK_BOOL_FALSE:
            return std::make_unique<BoolExpNode>(false, make_srange(beg_pos));
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
            expect(Token::TOK_EOF, "Encountered invalid syntax, found binary"
                    " op without preceding compatible type expression");
            advance();
            return nullptr;
            break;

        case Token::TOK_NEG:
        case Token::TOK_NOT:
        case Token::TOK_DEREF:
        case Token::TOK_ADDROF: {
            auto unop = conv_unop(tok_list[curr_pos]);
            advance();
            return std::make_unique<UnopExpNode>(
                unop, parse_expr(prev_prec),
                make_srange(beg_pos)
            );
        }

        default:
            expect(Token::TOK_EOF, "Encountered invalid syntax");
            advance();
            return nullptr;
    }
}

std::pair<Type*, std::string> Parser::parse_arg() {

    auto p_type = parse_type();

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& p_name = tok_list[curr_pos].lexeme;

    advance();

    if (tok_list[curr_pos].tok_ty != Token::TOK_RPAREN) {
        expect(Token::TOK_COMMA, "`,` to delimit function paramaters");
        advance();
    }

    return {p_type, p_name};
}


std::unique_ptr<ExpNode> Parser::parse_new_type_exp() {
    ssize_t beg_pos = curr_pos;

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

        return std::make_unique<NewExpNode>(
            std::move(p_type_uptr), std::move(size),
            make_srange(beg_pos)
        );
    }

    return std::make_unique<NewExpNode>(
        std::move(p_type_uptr), nullptr,
        make_srange(beg_pos)
    );
}

StmtNode* Parser::parse_vdecl() {
    ssize_t type_pos = curr_pos;
    std::unique_ptr<Type> vtype_ptr(parse_type());

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& vname = tok_list[curr_pos].lexeme;

    advance();

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

        ssize_t orig = curr_pos;
        curr_pos = type_pos;
        std::unique_ptr<Type> elem_type(parse_type_wo_arr());
        curr_pos = orig;

        auto srange = make_srange(type_pos);

        auto carr_exp = std::make_unique<CArrExpNode>(
            std::move(elem_type), init_exps,
            srange
        );

        return new DeclStmtNode(std::move(vtype_ptr), vname, std::move(carr_exp),
                                srange);
    }

    expect(Token::TOK_EQUAL, "`=` operator");
    advance();

    if (tok_list[curr_pos].tok_ty == Token::TOK_NEW) {
        advance();

        auto new_exp = parse_new_type_exp();
        advance();
        return new DeclStmtNode(
            std::move(vtype_ptr), vname, std::move(new_exp),
            make_srange(type_pos));
    }

    std::unique_ptr<ExpNode> init_exp = parse_expr(0);
    advance();

    return new DeclStmtNode(
        std::move(vtype_ptr), vname, std::move(init_exp),
        make_srange(type_pos));
}

StmtNode* Parser::parse_sfun_call() {
    ssize_t beg_pos = curr_pos;

    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_IDENT: {
            std::string& fun_name = tok_list[curr_pos].lexeme;

            advance();

            expect(Token::TOK_LPAREN, "`(` for a function call");

            advance();

            std::vector<ExpNode*> f_args {};

            while ((curr_pos < tok_len) &&
                    (tok_list[curr_pos].tok_ty != Token::TOK_RPAREN)) {
                f_args.push_back(parse_expr(0).release());
                advance();

                if (tok_list[curr_pos].tok_ty == Token::TOK_RPAREN)
                    break;

                expect(Token::TOK_COMMA, "`,` operator to separate function"
                        " arguments");
                advance();
            }

            advance();
            expect(Token::TOK_SEMIC, "terminating `;` operator");

            return new SCallStmtNode(fun_name, f_args, make_srange(beg_pos));
        }

        default:
            expect(Token::TOK_EOF, "Encountered invalid syntax");
            advance();
            return nullptr;
    }
}

StmtNode* Parser::parse_stmt() {
    ssize_t beg_pos = curr_pos;

    switch (tok_list[curr_pos].tok_ty) {
        case Token::TOK_TYPE_INT:
        case Token::TOK_TYPE_STRING:
        case Token::TOK_TYPE_BOOL: {
            auto decl = parse_vdecl();
            expect(Token::TOK_SEMIC, "terminating `;` operator");
            return decl;
        }

        case Token::TOK_RETURN:
            advance();

            if (tok_list[curr_pos].tok_ty == Token::TOK_SEMIC) {
                advance();
                expect(Token::TOK_SEMIC, "terminating `;` operator");
                return new RetStmtNode(nullptr, make_srange(beg_pos));
            }
            else{
                auto ret_node = new RetStmtNode(parse_expr(0), make_srange(beg_pos));
                advance();
                expect(Token::TOK_SEMIC, "terminating `;` operator");
                return ret_node;
            }
            break;

        case Token::TOK_IDENT: {

            ssize_t orig = curr_pos;

            auto left = parse_lhs(0);
            advance();

            if (tok_list[curr_pos].tok_ty != Token::TOK_EQUAL) {
                curr_pos = orig;
                return parse_sfun_call();
            }

            expect(Token::TOK_EQUAL, "`=` operator for assign statement");

            advance();

            auto right = parse_expr(0);

            advance();
            expect(Token::TOK_SEMIC, "terminating `;` operator");
            return new AssnStmtNode(std::move(left), std::move(right),
                make_srange(beg_pos));
        }

        case Token::TOK_IF: {
            advance();

            expect(Token::TOK_LPAREN, "`(` for condition expression");
            advance();

            auto cond = parse_expr(0);
            advance();

            expect(Token::TOK_RPAREN, "`)` for condition expression");

            advance();
            expect(Token::TOK_LBRACE, "`{` for if-then body");

            advance();

            auto then_body = parse_block();

            if (check_next() == Token::TOK_ELSE) {
                // Consume TOK_ELSE
                advance();

                advance();
                expect(Token::TOK_LBRACE, "`{` for if-then body");

                advance();

                auto else_body = parse_block();

                return new IfStmtNode(std::move(cond), then_body, else_body,
                    make_srange(beg_pos));
            }

            std::vector<StmtNode*> else_body {};
            return new IfStmtNode(std::move(cond), then_body, else_body,
                make_srange(beg_pos));
        }

        case Token::TOK_FOR: {
            advance();

            expect(Token::TOK_LPAREN, "`(` for `for` stmt");
            advance();

            std::vector<StmtNode*> decls;
            while ((curr_pos < tok_len) &&
                    tok_list[curr_pos].tok_ty != Token::TOK_SEMIC) {
                decls.push_back(parse_vdecl());
            }

            advance();

            auto cond = parse_expr(0);
            advance();

            expect(Token::TOK_SEMIC, "`;` for delimiting condition and iter in"
                                     " `for` loop");
            advance();

            auto iter = parse_stmt();
            advance();

            expect(Token::TOK_RPAREN, "`)` for ending `for` prelude");
            advance();

            expect(Token::TOK_LBRACE, "`{` for `for` body");
            advance();

            auto for_body = parse_block();

            std::unique_ptr<StmtNode> iter_uptr(iter);
            return new ForStmtNode(decls, std::move(cond), std::move(iter_uptr),
                                    for_body, make_srange(beg_pos));
        }

        case Token::TOK_WHILE: {
            advance();

            expect(Token::TOK_LPAREN, "`(` for `while` stmt");
            advance();

            auto cond = parse_expr(0);
            advance();

            expect(Token::TOK_RPAREN, "`)` for ending `while` prelude");
            advance();

            expect(Token::TOK_LBRACE, "`{` for `for` body");
            advance();

            auto while_body = parse_block();

            return new WhileStmtNode(std::move(cond), while_body,
                make_srange(beg_pos));
        }

        default:
            expect(Token::TOK_EOF, "Encountered invalid syntax");
            advance();
            return nullptr;
    }
}

std::vector<StmtNode*> Parser::parse_block() {
    std::vector<StmtNode*> stmts {};
    while (curr_pos < tok_len && tok_list[curr_pos].tok_ty != Token::TOK_RBRACE) {
        stmts.push_back(parse_stmt());
        advance();
    }
    return stmts;
}

// fdecl ::== fun <ident> (arg1,...,argn) { <stmt1>; <stmt2>; ... }
Decls* Parser::parse_fdecl() {
    ssize_t beg_pos = curr_pos;

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
                params.push_back(parse_arg());
                break;

            default:
                expect(Token::TOK_EOF, "Encountered invalid syntax for function parameters");
                advance();
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

    std::vector<StmtNode*> fn_body = parse_block();

    expect(Token::TOK_RBRACE, "`}` for ending function definition");
    advance();

    std::unique_ptr<Type> fret_type_uptr(fret_type);
    return new FunDecl(std::move(fret_type_uptr), f_name, params, fn_body,
        make_srange(beg_pos));
}

// vdecl ::== <type> : <ident> = <exp>
Decls* Parser::parse_gvdecl() {
    ssize_t beg_pos = curr_pos;

    std::unique_ptr<Type> gvtype_ptr(parse_type());

    advance();

    expect(Token::TOK_COLON, "`:` operator");
    advance();

    expect(Token::TOK_IDENT, "identifier");

    std::string& gvname = tok_list[curr_pos].lexeme;

    advance();

    expect(Token::TOK_EQUAL, "`=` operator");
    advance();

    std::unique_ptr<ExpNode> init_exp = parse_expr(0);
    advance();

    expect(Token::TOK_SEMIC, "terminating `;` operator");
    advance();

    return new GlobalDecl(std::move(gvtype_ptr), gvname, std::move(init_exp),
        make_srange(beg_pos));
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
            expect(Token::TOK_EOF, "Encountered invalid syntax top level" 
                    "declaration");
            advance();
            return nullptr;
            break;
    }
}

void Parser::parse_prog() {
    while (curr_pos < tok_len)
        prog.decl_list.push_back(parse_decl());
}
