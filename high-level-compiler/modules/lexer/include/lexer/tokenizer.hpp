#pragma once
#include <string>
#include <expected>
#include <vector>
#include "lexer/tokens.hpp"
#include <optional>
#include <unordered_map>

namespace lexer {

enum class TokenizerEC {
    UNDEFINED_TOKEN,
    INTERNAL_ERROR,
};

class Tokenizer {
public:
    explicit Tokenizer(std::string text);
    std::expected<std::vector<Token>, TokenizerEC> tokenize() noexcept;
private:
    std::optional<char> next(unsigned int shift=1) noexcept;
    std::optional<char> peek(unsigned int offset=0) const noexcept;
    std::optional<Token> parse_token_char() noexcept;

    bool is_space(char c) const noexcept;
    bool valid_id_symbol(char c) const noexcept;

    SourceLocation get_source() const noexcept;

    uint64_t m_curr=0;
    uint64_t m_line=1;
    uint64_t m_col=1;

    std::string m_text;

    inline static const std::unordered_map<std::string_view, TokenType> m_token_chars = {
        {";", TokenType::SEMICOLON},

        {"+", TokenType::OPERATOR_ADD},
        {"-", TokenType::OPERATOR_SUB},
        {"*", TokenType::OPERATOR_MUL},
        {"=", TokenType::OPERATOR_ASSIGN},

        {"+=", TokenType::OPERATOR_ADD_ASSIGN},
        {"-=", TokenType::OPERATOR_SUB_ASSIGN},
        {"*=", TokenType::OPERATOR_MUL_ASSIGN},
        {"==", TokenType::OPERATOR_EQUAL},

        {"(", TokenType::LPAREN},
        {")", TokenType::RPAREN},
        {"{", TokenType::LBRACE},
        {"}", TokenType::RBRACE},
        {",", TokenType::COMMA}
    };

    inline static const std::unordered_map<std::string_view, TokenType> m_keywords = {
        {"int", TokenType::KEYWORD_INT},
        {"void", TokenType::KEYWORD_VOID},
        {"return", TokenType::KEYWORD_RETURN},
        {"fn", TokenType::KEYWORD_FN},
    };

};

} // namespace lexer
