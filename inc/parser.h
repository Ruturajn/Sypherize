#ifndef __PARSER_H__
#define __PARSER_H__

#include "./ast.h"
#include "./token.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Parser {
public:
    std::vector<Token> tok_list;
    ssize_t curr_pos;
    ssize_t tok_len;
    std::unordered_map<Token::TokType, int> precedence;

    // This map binds function names to their local scope, which itself is a map
    // that binds variables used in the local scope to their type.
    // Global variables are available in all scopes, using the "__global__"
    // key, in the map.
    std::unordered_map<std::string,
        std::unordered_map<std::string, Type*>> env;

    static const std::string global_key;

    Program prog;

    enum DeclType {
        DECL_TYPE_INT,
        DECL_TYPE_STRING,
        DECL_TYPE_BOOL,
    };

    Parser(std::vector<Token>& _tok_list)
        : tok_list(_tok_list), curr_pos(0), tok_len(_tok_list.size()),
            precedence({}), prog(Program()) {

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

    Token::TokType check_next() const {
        if (curr_pos >= tok_len)
            return Token::TOK_EOF;

        if ((curr_pos + 1) >= tok_len)
            return Token::TOK_EOF;

        return tok_list[curr_pos + 1].tok_ty;
    }

    bool is_next_binop() const {
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

    int get_precedence(enum Token::TokType t_ty) {
        if (precedence.find(t_ty) != precedence.end())
            return precedence[t_ty];

        /* std::cout << "[ERR]: Encountered invalid binary op at: " << */
        /*     "[" << tok_list[curr_pos].line_num << "," << */
        /*     tok_list[curr_pos].col_num << "]\n"; */

        return -1;
    }

    enum BinopExpNode::BinopType conv_binop(const Token& t) const {
        switch (t.tok_ty) {
            case Token::TOK_PLUS:
                return BinopExpNode::BINOP_PLUS;

            case Token::TOK_MINUS:
                return BinopExpNode::BINOP_MINUS;

            case Token::TOK_MULT:
                return BinopExpNode::BINOP_MULT;

            case Token::TOK_DIV:
                return BinopExpNode::BINOP_DIVIDE;

            case Token::TOK_LSHIFT:
                return BinopExpNode::BINOP_LSHIFT;

            case Token::TOK_RSHIFT:
                return BinopExpNode::BINOP_RSHIFT;

            case Token::TOK_MODULUS:
                return BinopExpNode::BINOP_MODULUS;

            case Token::TOK_BITAND:
                return BinopExpNode::BINOP_BITAND;

            case Token::TOK_BITOR:
                return BinopExpNode::BINOP_BITOR;

            case Token::TOK_BITXOR:
                return BinopExpNode::BINOP_BITXOR;

            case Token::TOK_EQEQUAL:
                return BinopExpNode::BINOP_EQEQUAL;

            case Token::TOK_NEQUAL:
                return BinopExpNode::BINOP_NEQUAL;

            case Token::TOK_GT:
                return BinopExpNode::BINOP_GT;

            case Token::TOK_LT:
                return BinopExpNode::BINOP_LT;

            case Token::TOK_GTE:
                return BinopExpNode::BINOP_GTE;

            case Token::TOK_LTE:
                return BinopExpNode::BINOP_LTE;

            case Token::TOK_LOGAND:
                return BinopExpNode::BINOP_LOGAND;

            case Token::TOK_LOGOR:
                return BinopExpNode::BINOP_LOGOR;

            default:
                std::cout << "[ERR]: Encountered invalid token for binop "
                    "conversion at: " <<
                    "[" << t.line_num << "," << t.col_num << "]\n";
                return BinopExpNode::BINOP_PLUS;
        }
    }

    void advance() { curr_pos += 1; }

    bool expect(Token::TokType t_ty, const char* expected) {
        if (tok_list[curr_pos].tok_ty != t_ty) {
            std::cout << "[ERR]: Expected " << expected << " at: " <<
                "[" << tok_list[curr_pos].line_num << "," <<
                tok_list[curr_pos].col_num << "]\n";
            return false;
        }

        return true;
    }

    void var_bind_ctxt(const std::string& fun_ctxt,
                       const std::string& var_name,
                       Type* val) {

        if (env.find(fun_ctxt) == env.end())
            env[fun_ctxt] = {};

        if (env[fun_ctxt].find(var_name) != env[fun_ctxt].end())
            std::cout << "[ERR]: Redefinition of variable " << var_name <<
                         " at: " <<
                         "[" << tok_list[curr_pos].line_num << "," <<
                         tok_list[curr_pos].col_num << "]\n";

        env[fun_ctxt][var_name] = val;
    }

    Type* parse_type(int in_count, enum DeclType dt) {
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

        return new TRef(parse_type(in_count - 1,dt));
    }

    std::unique_ptr<ExpNode> parse_binop(int prev_prec,
                                         std::unique_ptr<ExpNode> lhs);
    std::unique_ptr<ExpNode> parse_expr(int prev_prec);
    std::unique_ptr<StmtNode> parse_stmt();
    std::pair<std::unique_ptr<Type>, std::string> parse_arg();
    Decls* parse_gvdecl();
    Decls* parse_fdecl();
    Decls* parse_decl();
    void parse_prog();
};

#endif // __PARSER_H__
