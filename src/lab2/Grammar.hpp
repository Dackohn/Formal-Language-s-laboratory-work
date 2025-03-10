#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

class Grammar {
public:
    using Symbol = std::string;
    using ProductionMap = std::unordered_map<Symbol, std::vector<std::string>>;

    std::unordered_set<Symbol> NonTerminals;
    std::unordered_set<Symbol> Terminals;
    ProductionMap Productions;
    Symbol StartSymbol;
    static inline std::mt19937 rng{ std::random_device{}() };

    Grammar(
        const std::unordered_set<Symbol>& nonTerminals,
        const std::unordered_set<Symbol>& terminals,
        const ProductionMap& productions,
        const Symbol& startSymbol)
        : NonTerminals(nonTerminals), Terminals(terminals), Productions(productions), StartSymbol(startSymbol) {}

    std::vector<std::string> GenerateStrings(int count = 5) {
        std::vector<std::string> generatedStrings;
        for (int i = 0; i < count; i++) {
            std::string generatedWord = GenerateString(StartSymbol);
            std::cout << generatedWord << std::endl;
            generatedStrings.push_back(generatedWord);
        }
        return generatedStrings;
    }

private:
    std::string GenerateString(const Symbol& symbol) {
        if (Terminals.find(symbol) != Terminals.end()) {
            return symbol;
        }

        if (Productions.find(symbol) != Productions.end()) {
            auto& rules = Productions[symbol];
            std::uniform_int_distribution<int> distrib(0, rules.size() - 1);
            std::string rule = rules[distrib(rng)];

            std::string result;
            for (char c : rule) {
                std::string symbolStr(1, c); 

                if (Terminals.find(symbolStr) != Terminals.end()) {
                    result += symbolStr;
                }
                else if (NonTerminals.find(symbolStr) != NonTerminals.end()) {
                    result += GenerateString(symbolStr);
                }
            }
            return result;
        }

        return symbol;
    }

public:
    std::string ClassifyGrammar() {
        bool isRegular = true;
        bool isContextFree = true;
        bool isContextSensitive = true;

        for (const auto& [left, rules] : Productions) {
            for (const auto& right : rules) {
                if (right.empty()) continue;

                std::vector<std::string> rhsSymbols;
                std::string currentSymbol;
                
                for (char c : right) {
                    if (c == ' ') {
                        if (!currentSymbol.empty()) {
                            rhsSymbols.push_back(currentSymbol);
                            currentSymbol.clear();
                        }
                    } else {
                        currentSymbol += c;
                    }
                }
                if (!currentSymbol.empty()) rhsSymbols.push_back(currentSymbol);

                if (rhsSymbols.size() > 2 || (rhsSymbols.size() == 2 && !NonTerminal(rhsSymbols[1][0]))) {
                    isRegular = false;
                }

                if (left.length() > 1 || !NonTerminal(left[0])) {
                    isContextFree = false;
                }

                if (left.length() > right.length()) {
                    isContextSensitive = false;
                }
            }
        }

        if (isRegular) return "Type 3: Regular Grammar";
        if (isContextFree) return "Type 2: Context-Free Grammar";
        if (isContextSensitive) return "Type 1: Context-Sensitive Grammar";
        return "Type 0: Unrestricted Grammar";
    }

private:
    static bool NonTerminal(char c) {
        return c >= 'A' && c <= 'Z';
    }
};

#endif
