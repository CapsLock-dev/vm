#include <catch2/catch_test_macros.hpp>
#include "lexer/tokenizer.hpp"
#include <string>

TEST_CASE("Tokenizer basic example", "[lexer]") {
    using namespace lexer;
    std::expected<std::vector<Token>, TokenizerEC> res;
    {
        std::string text = "fn int main() {\nreturn 0;\n}";
        Tokenizer tokenizer{text};
        res = tokenizer.tokenize();
        REQUIRE(res.has_value());
        REQUIRE(!res->empty());
    }
    std::vector<Token> want = {
        {.type=TokenType::KEYWORD_FN, .value="fn", .source={}},
        {.type=TokenType::KEYWORD_INT, .value="int", .source={}},
        {.type=TokenType::IDENTIFIER, .value="main", .source={}},
        {.type=TokenType::LPAREN, .value="(", .source={}},
        {.type=TokenType::RPAREN, .value=")", .source={}},
        {.type=TokenType::LBRACE, .value="{", .source={}},
        {.type=TokenType::KEYWORD_RETURN, .value="return", .source={}},
        {.type=TokenType::INT_LITERAL, .value="0", .source={}},
        {.type=TokenType::SEMICOLON, .value=";", .source={}},
        {.type=TokenType::RBRACE, .value="}", .source={}},
    };
    REQUIRE(res->size() == want.size());
    for (size_t i=0; i<res->size(); ++i) {
        REQUIRE((*res)[i].type == want[i].type);
        REQUIRE((*res)[i].value == want[i].value);
    }
}

TEST_CASE("Tokenizer valid identifier", "[lexer]") {
    using namespace lexer;
    std::expected<std::vector<Token>, TokenizerEC> res;
    {
        std::string text = "int abc_13;";
        Tokenizer tokenizer{text};
        res = tokenizer.tokenize();
        REQUIRE(res.has_value());
        REQUIRE(!res->empty());
    }
    std::vector<Token> want = {
        {.type=TokenType::KEYWORD_INT, .value="int", .source={}},
        {.type=TokenType::IDENTIFIER, .value="abc_13", .source={}},
        {.type=TokenType::SEMICOLON, .value=";", .source={}},
    };
    REQUIRE(res->size() == want.size());
    for (size_t i=0; i<res->size(); ++i) {
        REQUIRE((*res)[i].type == want[i].type);
        REQUIRE((*res)[i].value == want[i].value);
    }
}
