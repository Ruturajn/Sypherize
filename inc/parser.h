#ifndef __PARSER_H__
#define __PARSER_H__

#include "./ast.h"
#include "./token.h"
#include <iomanip>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

class Parser {
public:
    std::string file_name;
    std::vector<std::string> file_lines;
    std::vector<Token> tok_list;
    ssize_t curr_pos;
    ssize_t tok_len;
    std::unordered_map<Token::TokType, int> precedence;
    Program prog;
    bool failed;
    enum DeclType {
        DECL_TYPE_INT,
        DECL_TYPE_STRING,
        DECL_TYPE_BOOL,
    };

    void advance() { curr_pos += 1; }
    enum DeclType conv_type(enum Token::TokType t_ty);
    Parser(std::vector<Token>& _tok_list, const std::string& _file_data,
            const std::string& _file_name);
    void expect(Token::TokType t_ty, const char* error_str, const Token& tok);
    void expect(Token::TokType t_ty, const char* error_str);
    Token::TokType check_next() const;
    bool is_next_binop() const;
    int get_precedence(enum Token::TokType t_ty);
    enum BinopExpNode::BinopType conv_binop(const Token& t);
    enum UnopExpNode::UnopType conv_unop(const Token& t);
    void var_bind_ctxt(const std::string& fun_ctxt,
                       const std::string& var_name,
                       Type* val);
    Type* build_type(int in_count, enum DeclType dt);
    Type* parse_type();
    Type* parse_type_wo_arr();
    std::unique_ptr<ExpNode> parse_new_type_exp();
    std::unique_ptr<ExpNode> parse_binop(int prev_prec,
                                         std::unique_ptr<ExpNode> lhs);
    std::unique_ptr<ExpNode> parse_expr(int prev_prec);
    std::unique_ptr<ExpNode> parse_lhs(int prev_prec);
    StmtNode* parse_vdecl();
    StmtNode* parse_sfun_call();
    StmtNode* parse_stmt();
    std::pair<Type*, std::string> parse_arg();
    std::vector<StmtNode*> parse_block();
    Decls* parse_fdecl();
    Decls* parse_gvdecl();
    Decls* parse_decl();
    void parse_prog();
};

#endif // __PARSER_H__
