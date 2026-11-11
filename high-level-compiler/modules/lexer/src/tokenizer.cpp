#include "lexer/tokenizer.hpp"
#include <print>
#include <unordered_map>
#include <string_view>
#include <ranges>

namespace lexer {

std::expected<std::vector<Token>, TokenizerError> Tokenizer::tokenize(std::string_view line) {
    //FIXME: Если строка заданная юзером будет содержать пробелы, то токенайзер её тоже попытается разбить на токены
    for (auto word : std::views::split(line, " ")) {
        std::println("{}", word);
    }
    return {};
}

TokenType Tokenizer::str_to_token(std::string_view token) {
    std::unordered_map<std::string, TokenType> mappings = {
        {"int", TokenType::KEYWORD_INT},
        {"return", TokenType::KEYWORD_RETURN},
        {"void", TokenType::KEYWORD_VOID},
        {"+", TokenType::OPERATOR_ADD},
        {"-", TokenType::OPERATOR_SUB},
        {"/", TokenType::OPERATOR_DIV},
        {"*", TokenType::OPERATOR_MUL},
        {"=", TokenType::OPERATOR_ASSIGN},
    };
    return TokenType::UNDEFINED; 
}

} // namespace lexer
