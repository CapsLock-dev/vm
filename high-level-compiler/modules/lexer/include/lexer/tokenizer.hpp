#pragma once
#include <string>
#include <expected>
#include <vector>
#include "lexer/tokens.hpp"

namespace lexer {

enum class TokenizerEC {
    UNDEFINED,
    UNDEFINED_TOKEN,
};

struct TokenizerError {
    TokenizerEC code;
    std::string message;
};

class Tokenizer {
public:
    Tokenizer() = default;
    std::expected<std::vector<Token>, TokenizerError> tokenize(std::string_view line);
private:
    TokenType str_to_token(std::string_view token);
};

} // namespace lexer
