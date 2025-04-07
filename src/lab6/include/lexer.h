#pragma once
#include "token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

class Lexer {
public:
    explicit Lexer(const std::string& input);
    std::vector<Token> tokenize();
    const std::vector<Token>& getInvalidTokens() const;

private:
    std::string input;
    size_t pos = 0;
    int line = 1;
    int column = 1;

    std::vector<Token> invalid_tokens;

    std::vector<std::pair<TokenType, std::string>> tokenPatterns;
    std::unordered_map<std::string, TokenType> keywords;

    void initTokenPatterns();
    void updatePosition(const std::string& value);
};
