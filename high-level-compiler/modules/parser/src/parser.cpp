#include "parser/parser.hpp"
#include "core/logger.hpp"

namespace parser {

using TokenType = lexer::TokenType;
using Token = lexer::Token;

static std::unexpected<ParserErr> err_unexpected_token(const std::string& val) {
    return std::unexpected<ParserErr>({"Unexpected token "+val}) ;
}

static std::unexpected<ParserErr> err_no_token() {
    return std::unexpected<ParserErr>({"No token"}) ;
}

Parser::Parser(std::vector<lexer::Token>&& tokens)
    : m_tokens{std::move(tokens)} 
    , m_arena{256}
{}

void Parser::parse() {
    ast::Program prog{ {} };  
    while (peek().has_value()) {
        TokenType type = peek().value().type; 
        switch (type) {
            case TokenType::KEYWORD_FN: {
                auto fn = parse_func_decl();
                auto source = peek().value().source;
                if (!fn) {
                    LOG_ERROR("Failed to parse function {}. At: {}:{}", fn.error().message, source.line, source.column);
                    return;
                }
                prog.decls.push_back(*fn);
                break;
            }
            case TokenType::KEYWORD_INT: {
                auto int_decl = parse_int_decl();
                auto source = peek().value().source;
                if (!int_decl) {
                    LOG_ERROR("Failed to parse int decl {}. At: {}:{}", int_decl.error().message, source.line, source.column);
                    return;
                }
                prog.decls.push_back(*int_decl);
                break;
            }
            default:
                auto source = peek().value().source;
                LOG_ERROR("Unexpected token at: {}:{}", source.line, source.column);
                break;
        }
    }
}

std::expected<ast::FuncDecl*, ParserErr> Parser::parse_func_decl() {
    ast::FuncDecl* f = m_arena.alloc<ast::FuncDecl>();
    auto curr = consume();

    if (!curr.has_value()) return err_no_token();
    switch (curr->type) {
        case TokenType::KEYWORD_INT:
            f->return_type = ast::Types::INT;
            break;
        case TokenType::KEYWORD_VOID:
            f->return_type = ast::Types::VOID;
            break;
        default:
            return nullptr;
    }
    curr = consume();

    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::IDENTIFIER) return err_unexpected_token(curr->value);

    f->name = curr->value;

    curr = consume();
    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::LPAREN) return err_unexpected_token(curr->value);

    //TODO: parse arguments

    curr = consume();
    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::RPAREN) return err_unexpected_token(curr->value);

    auto block = parse_block();
    if (!block) return std::unexpected(block.error());
 
    LOG_DEBUG("Parsed function {} {}", static_cast<int>(f->return_type), f->name);

    return f;
}

std::expected<ast::Block*, ParserErr> Parser::parse_block() {
    ast::Block* block = m_arena.alloc<ast::Block>();

    auto curr = consume(); 
    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::LBRACE) return err_unexpected_token(curr->value);

    switch (curr->type) {
        case TokenType::KEYWORD_INT: {
            auto int_decl = parse_int_decl();
            if (int_decl == nullptr) return std::unexpected(int_decl.error());
            break;                                  
        }
        case TokenType::KEYWORD_RETURN: {
            auto int_decl = parse_ret_stmt();
            if (int_decl == nullptr) return std::unexpected(int_decl.error());
            break;                                  
        }
        default:
            return err_unexpected_token(curr->value);
    }

    LOG_DEBUG("Parsed block");

    return block;
}

std::expected<ast::IntDecl*, ParserErr> Parser::parse_int_decl() {
    ast::IntDecl* int_decl = m_arena.alloc<ast::IntDecl>();

    auto curr = consume();
    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::IDENTIFIER) return err_unexpected_token(curr->value);
    int_decl->name = curr->value;

    curr = consume();
    if (!curr.has_value()) return err_no_token();
    if (curr->type == TokenType::IDENTIFIER || curr->type == TokenType::INT_LITERAL) {
    }

    curr = consume();
    if (!curr.has_value()) return err_no_token();
    if (curr->type != TokenType::SEMICOLON) return err_unexpected_token(curr->value);

    LOG_DEBUG("Parsed int decl {}", int_decl->name);
    return int_decl;
}

std::expected<ast::ReturnStmt*, ParserErr> Parser::parse_ret_stmt() {

}

std::expected<ast::BinExpr*, ParserErr> Parser::parse_expression() {

}

} //namespace parser
