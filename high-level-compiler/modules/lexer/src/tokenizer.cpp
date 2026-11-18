#include "lexer/tokenizer.hpp"
#include "lexer/tokens.hpp"
#include "core/logger.hpp"
#include <cctype>
#include <unordered_map>
#include <string_view>
#include <utility>

namespace lexer {

Tokenizer::Tokenizer(std::string text)
: m_text{std::move(text)} {}

std::expected<std::vector<Token>, TokenizerEC> Tokenizer::tokenize() noexcept {
    std::vector<Token> tokens;
    std::string_view text_view{m_text};
    while (peek().has_value()) {
        while (peek().has_value() && is_space(peek().value())) {
            next();
        }
        if (!peek().has_value()) break;

        char curr = peek().value();
        std::string_view token{};

        if (std::isalpha(curr)) {
            uint64_t start = m_curr;
            while (peek().has_value() && std::isalpha(peek().value())) next();
            token = text_view.substr(start, m_curr-start);
            LOG_INFO("[Tokenizer] Parsed token at {}:{} - {}", m_line, m_col, token);
            
            if (m_keywords.contains(token)) {
                tokens.emplace_back(Token{.type=m_keywords.at(token), .value=std::string(token), .source=get_source()});
            } else {
                tokens.emplace_back(Token{.type=TokenType::IDENTIFIER, .value=std::string(token), .source=get_source()});
            }

        } else if (std::isdigit(curr)) {
            uint64_t start = m_curr;
            while (peek().has_value() && std::isdigit(peek().value())) next();
            token = text_view.substr(start, m_curr-start);
            LOG_INFO("[Tokenizer] Parsed token at {}:{} - {}", m_line, m_col, token); 
            tokens.emplace_back(Token{.type = TokenType::INT_LITERAL, .value=std::string(token), .source=get_source()}) ;

        } else if (m_token_chars.contains(text_view.substr(m_curr, 1))) { 
            LOG_INFO("[Tokenizer] Parsed token at {}:{} - {}", m_line, m_col, curr);
            std::optional<Token> tok = parse_token_char();
            if (!tok.has_value()) {
                LOG_ERROR("[Tokenizer] Parsing failed at {}:{} - {}", m_line, m_col, curr);
                return std::unexpected(TokenizerEC::INTERNAL_ERROR);
            }
            tokens.push_back(std::move(tok.value()));

        } else {
            LOG_ERROR("[Tokenizer] Undefined symbol at {}:{} - {}", m_line, m_col, curr);
            return std::unexpected(TokenizerEC::UNDEFINED_TOKEN);
        }
    } 
    
    return tokens;
}

std::optional<TokenType> Tokenizer::str_to_token(std::string_view token) const noexcept {
    static const std::unordered_map<std::string, TokenType> mappings = {
        {"int", TokenType::KEYWORD_INT},
        {"return", TokenType::KEYWORD_RETURN},
        {"void", TokenType::KEYWORD_VOID},
    };
    auto it = mappings.find(std::string(token));
    if (it == mappings.end()) return std::nullopt;
    return it->second;
}

std::optional<char> Tokenizer::peek(unsigned int offset) const noexcept {
    if (m_curr+offset >= m_text.length()) return std::nullopt;
    return m_text[m_curr+offset];
}

std::optional<char> Tokenizer::next(unsigned int shift) noexcept {
    if (m_curr+shift > m_text.length()) return std::nullopt;
    m_col += shift;
    if (m_text[m_curr] == '\n') {
        ++m_line;
        m_col = 1;
    }
    return m_text[m_curr++];
}

std::optional<Token> Tokenizer::parse_token_char() noexcept {
    std::string_view view{m_text};
    if (peek(1).has_value() && peek(1).value() == '=') {
        view = view.substr(m_curr, 2);
        next(2);
    } else {
        view = view.substr(m_curr, 1);
        next(1);
    }
    if (!m_token_chars.contains(view)) return std::nullopt;
    return Token{.type = m_token_chars.at(view), .value=std::string(view), .source=get_source()};
}

SourceLocation Tokenizer::get_source() const noexcept {
    return {.offset = m_curr, .line = m_line, .column = m_col};
}

bool Tokenizer::is_space(char c) const noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

} // namespace lexer
