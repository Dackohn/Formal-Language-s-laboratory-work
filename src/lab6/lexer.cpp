#include "include/lexer.h"
#include <regex>
#include <cctype>

Lexer::Lexer(const std::string& input) : input(input) {
    initTokenPatterns();
}

void Lexer::initTokenPatterns() {
    tokenPatterns = {
        {TokenType::EQUAL_EQUAL, R"(==)"},
        {TokenType::LESS_EQUAL, R"(<=)"},
        {TokenType::GREATER_EQUAL, R"(>=)"},
        {TokenType::NOT_EQUAL, R"(!=)"},
        {TokenType::ARROW, R"(->)"},
        {TokenType::EQUAL, R"(=)"},
        {TokenType::LESS, R"(<)"},
        {TokenType::GREATER, R"(>)"},
        {TokenType::LBRACE, R"(\{)"},
        {TokenType::RBRACE, R"(\})"},
        {TokenType::LPAREN, R"(\()"},
        {TokenType::RPAREN, R"(\))"},
        {TokenType::LBRACKET, R"(\[)"},
        {TokenType::RBRACKET, R"(\])"},
        {TokenType::COMMA, R"(,)"},
        {TokenType::DOT, R"(\.)"},
        {TokenType::STRING, R"str("([^"\n]*)")str"},
        {TokenType::TIME_UNIT, R"(\d+(d|h|m))"}, // e.g. 7d
        {TokenType::FLOAT, R"(\d+\.\d+)"},       // e.g. 12.5
        {TokenType::INT, R"(\d+)"},              // e.g. 12
        {TokenType::ID, R"([A-Za-z_][A-Za-z0-9_]*)"}
    };    

    keywords = {
        {"LOAD", TokenType::LOAD}, {"FROM", TokenType::FROM}, {"SET", TokenType::SET},
        {"WINDOW", TokenType::WINDOW}, {"TREND", TokenType::TREND}, {"FORECAST", TokenType::FORECAST},
        {"USING", TokenType::USING}, {"STREAM", TokenType::STREAM}, {"SELECT", TokenType::SELECT},
        {"WHERE", TokenType::WHERE}, {"DATE", TokenType::DATE}, {"PLOT", TokenType::PLOT},
        {"EXPORT", TokenType::EXPORT}, {"TO", TokenType::TO}, {"FOR", TokenType::FOR},
        {"IN", TokenType::IN}, {"REMOVE", TokenType::REMOVE}, {"MISSING", TokenType::MISSING},
        {"REPLACE", TokenType::REPLACE}, {"WITH", TokenType::WITH}, {"ANALYZE", TokenType::ANALYZE},
        {"BASED_ON", TokenType::BASED_ON}, {"BELOW", TokenType::BELOW}, {"ABOVE", TokenType::ABOVE},
        {"MEAN", TokenType::MEAN}, {"MEDIAN", TokenType::MEDIAN}, {"TENDENCY", TokenType::TENDENCY},
        {"ARIMA", TokenType::ARIMA}, {"PROPHET", TokenType::PROPHET}, {"LSTM", TokenType::LSTM},
        {"LINEPLOT", TokenType::LINEPLOT}, {"HISTOGRAM", TokenType::HISTOGRAM},
        {"SCATTERPLOT", TokenType::SCATTERPLOT}, {"BARPLOT", TokenType::BARPLOT}
    };
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    std::smatch match;

    while (pos < input.size()) {
        std::string remaining = input.substr(pos);
        bool matched = false;

        // Skip whitespace manually
        if (std::isspace(input[pos])) {
            updatePosition(std::string(1, input[pos]));
            continue;
        }

        for (const auto& [type, pattern] : tokenPatterns) {
            std::regex regex("^" + pattern);
            if (std::regex_search(remaining, match, regex)) {
                std::string value = match[0];

                TokenType finalType = type;
                if (type == TokenType::ID) {
                    std::string upper;
                    for (char c : value) upper += std::toupper(c);
                    if (keywords.count(upper)) {
                        finalType = keywords[upper];
                    }
                }

                tokens.emplace_back(finalType, value, line, column);
                updatePosition(value);
                matched = true;
                break;
            }
        }

        if (!matched) {
            // Skip invalid character silently
            updatePosition(std::string(1, input[pos]));
        }
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

void Lexer::updatePosition(const std::string& value) {
    for (char c : value) {
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

const std::vector<Token>& Lexer::getInvalidTokens() const {
    return invalid_tokens; // Will always be empty in this version
}
