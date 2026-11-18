#pragma once
#include <vector>
#include <optional>
#include <expected>
#include "lexer/tokens.hpp"
#include "parser/arena_allocator.hpp"
#include "parser/ast.hpp"

namespace parser {


struct ParserErr {
    std::string message;
};

class Parser {
public:
    explicit Parser(std::vector<lexer::Token>&& tokens);
    void parse();
private:
    std::expected<ast::ReturnStmt*, ParserErr> parse_ret_stmt();
    std::expected<ast::FuncDecl*, ParserErr> parse_func_decl();
    std::expected<ast::IntDecl*, ParserErr> parse_int_decl();
    std::expected<ast::BinExpr*, ParserErr> parse_expression();
    std::expected<ast::Block*, ParserErr> parse_block();

    std::optional<lexer::Token> peek(unsigned int offset=0) const noexcept;
    std::optional<lexer::Token> consume(unsigned int amount=1) noexcept;
    std::vector<lexer::Token> m_tokens;
    ArenaAllocator m_arena;
    uint64_t m_curr=0;  
};

} //namespace parser
