#include <catch2/catch_test_macros.hpp>
#include "lexer/tokenizer.hpp"
#include <string>

TEST_CASE("Tokenizer basic example", "[lexer]") {
    using namespace lexer;
    std::string text = "int main() {\nreturn 0;\n}";
    Tokenizer tokenizer{text};
    auto res = tokenizer.tokenize();
    REQUIRE(res.has_value());
    REQUIRE(!res->empty());
    std::vector<Token> want = {
        {.type=TokenType::KEYWORD_INT, .value="", .source={}},
        {.type=TokenType::IDENTIFIER, .value="main", .source={}},
        {.type=TokenType::LPAREN, .value="", .source={}},
        {.type=TokenType::RPAREN, .value="", .source={}},
        {.type=TokenType::LBRACE, .value="", .source={}},
        {.type=TokenType::KEYWORD_RETURN, .value="", .source={}},
        {.type=TokenType::INT_LITERAL, .value="0", .source={}},
        {.type=TokenType::SEMICOLON, .value="", .source={}},
        {.type=TokenType::RBRACE, .value="", .source={}},
    };
    REQUIRE(res->size() == want.size());
    for (size_t i=0; i<res->size(); ++i) {
        REQUIRE((*res)[i].type == want[i].type);
        REQUIRE((*res)[i].value == want[i].value);
    }
}
