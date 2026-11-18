#pragma once
#include <string>
#include <stdint.h>

namespace lexer {

struct SourceLocation {
    uint64_t offset=0; // in bytes from beggining of the file
    uint64_t line=1; // only used for error messages
    uint64_t column=1; // only used for error messages
};

enum class TokenType {
    UNDEFINED,
    EndOfFile,

    IDENTIFIER,
    INT_LITERAL,

    KEYWORD_INT,
    KEYWORD_FN,
    KEYWORD_VOID,
    KEYWORD_RETURN,

    OPERATOR_ADD,
    OPERATOR_SUB,
    OPERATOR_DIV,
    OPERATOR_MUL, 
    OPERATOR_ASSIGN, // = 

    OPERATOR_ADD_ASSIGN, // +=
    OPERATOR_SUB_ASSIGN, // -=
    OPERATOR_DIV_ASSIGN, // /=
    OPERATOR_MUL_ASSIGN, // *=
    
    OPERATOR_EQUAL, // == 

    LPAREN, RPAREN, // ( )
    LBRACE, RBRACE, // { }
    SEMICOLON // ;
};

struct Token {
    TokenType type;
    std::string value;
    SourceLocation source; // only used for error messages
};

} // namespace lexer
