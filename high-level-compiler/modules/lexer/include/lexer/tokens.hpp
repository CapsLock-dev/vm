#pragma once
#include <string>

namespace lexer {

enum class TokenType {
    UNDEFINED,

    IDENTIFIER,
    CONSTANT,

    KEYWORD_INT,
    KEYWORD_VOID,
    KEYWORD_RETURN,

    OPERATOR_ADD,
    OPERATOR_SUB,
    OPERATOR_DIV,
    OPERATOR_MUL,
    OPERATOR_ASSIGN,
};

struct Token {
    TokenType type;
    std::string value;
};

} // namespace lexer
