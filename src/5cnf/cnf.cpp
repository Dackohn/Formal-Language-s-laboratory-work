#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

class CNFConverter {
public:
    using Symbol = std::string;
    using Production = std::vector<Symbol>;
    using Grammar = std::unordered_map<Symbol, std::vector<Production>>;

    CNFConverter(const Grammar& input, const Symbol& start)
        : grammar(input), startSymbol(start), newVarCount(0) {}

    Grammar convertToCNF() {
        removeNullProductions();
        removeUnitProductions();
        removeUselessSymbols();
        convertTerminalsInRules();
        convertLongProductions();  
        return grammar;
    }

private:
    Grammar grammar;
    Symbol startSymbol;
    int newVarCount;

    Symbol getNewVariable() {
        return "X" + std::to_string(++newVarCount);
    }

    void removeNullProductions() {
        std::unordered_set<Symbol> nullable;

        for (const auto& [A, prods] : grammar) {
            for (const auto& prod : prods) {
                if (prod.size() == 1 && prod[0] == "~") {
                    nullable.insert(A);
                }
            }
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& [A, prods] : grammar) {
                for (const auto& prod : prods) {
                    if (std::all_of(prod.begin(), prod.end(), [&](const Symbol& s) { return nullable.count(s); })) {
                        if (!nullable.count(A)) {
                            nullable.insert(A);
                            changed = true;
                        }
                    }
                }
            }
        }

        Grammar newGrammar;
        for (const auto& [A, prods] : grammar) {
            for (const auto& prod : prods) {
                std::vector<Production> expansions = { {} };
                for (const auto& symbol : prod) {
                    std::vector<Production> temp;
                    for (const auto& e : expansions) {
                        Production with = e;
                        with.push_back(symbol);
                        temp.push_back(with);
                        if (nullable.count(symbol)) {
                            temp.push_back(e);
                        }
                    }
                    expansions = temp;
                }
                for (const auto& e : expansions) {
                    if (!e.empty() && !(e.size() == 1 && e[0] == "~")) {
                        newGrammar[A].push_back(e);
                    }
                }
            }
        }

        grammar = newGrammar;
    }

    void removeUnitProductions() {
        std::set<std::pair<Symbol, Symbol>> unitPairs;

        for (const auto& [A, prods] : grammar) {
            for (const auto& prod : prods) {
                if (prod.size() == 1 && isNonTerminal(prod[0])) {
                    unitPairs.emplace(A, prod[0]);
                }
            }
        }

        bool changed = true;
        while (changed) {
            changed = false;
            std::set<std::pair<Symbol, Symbol>> newPairs;
            for (const auto& [A, B] : unitPairs) {
                for (const auto& prod : grammar[B]) {
                    if (prod.size() == 1 && isNonTerminal(prod[0])) {
                        if (!unitPairs.count({ A, prod[0] })) {
                            newPairs.emplace(A, prod[0]);
                            changed = true;
                        }
                    }
                }
            }
            unitPairs.insert(newPairs.begin(), newPairs.end());
        }

        for (const auto& [A, B] : unitPairs) {
            for (const auto& prod : grammar[B]) {
                if (!(prod.size() == 1 && isNonTerminal(prod[0]))) {
                    grammar[A].push_back(prod);
                }
            }
        }

        for (auto& [A, prods] : grammar) {
            prods.erase(std::remove_if(prods.begin(), prods.end(),
                [&](const Production& prod) {
                    return prod.size() == 1 && isNonTerminal(prod[0]);
                }), prods.end());
        }
    }

    void removeUselessSymbols() {
        std::unordered_set<Symbol> generating;
        bool changed = true;

        while (changed) {
            changed = false;
            for (const auto& [A, prods] : grammar) {
                for (const auto& prod : prods) {
                    if (std::all_of(prod.begin(), prod.end(), [&](const Symbol& s) {
                        return !isNonTerminal(s) || generating.count(s);
                    })) {
                        if (!generating.count(A)) {
                            generating.insert(A);
                            changed = true;
                        }
                    }
                }
            }
        }

        std::unordered_set<Symbol> reachable = { startSymbol };
        changed = true;
        while (changed) {
            changed = false;
            for (const auto& A : reachable) {
                for (const auto& prod : grammar[A]) {
                    for (const auto& s : prod) {
                        if (isNonTerminal(s) && !reachable.count(s)) {
                            reachable.insert(s);
                            changed = true;
                        }
                    }
                }
            }
        }

        Grammar newGrammar;
        for (const auto& [A, prods] : grammar) {
            if (reachable.count(A) && generating.count(A)) {
                for (const auto& prod : prods) {
                    if (std::all_of(prod.begin(), prod.end(), [&](const Symbol& s) {
                        return !isNonTerminal(s) || (generating.count(s) && reachable.count(s));
                    })) {
                        newGrammar[A].push_back(prod);
                    }
                }
            }
        }

        grammar = newGrammar;
    }

    void convertLongProductions() {
    Grammar newGrammar;
    std::unordered_map<std::string, Symbol> pairToVar;
    std::unordered_map<Symbol, std::vector<Production>> intermediateRules;

    for (auto& [A, prods] : grammar) {
        for (auto& prod : prods) {
            if (prod.size() <= 2) {
                newGrammar[A].push_back(prod);
                continue;
            }

            std::vector<Symbol> symbols = prod;
            Symbol prev;

            while (symbols.size() > 2) {
                Symbol right = symbols.back(); symbols.pop_back();
                Symbol left = symbols.back(); symbols.pop_back();

                std::string key = left + "," + right;
                Symbol temp;
                if (pairToVar.count(key)) {
                    temp = pairToVar[key];
                } else {
                    temp = getNewVariable();
                    pairToVar[key] = temp;
                    intermediateRules[temp].push_back({ left, right });
                }
                symbols.push_back(temp);
            }

            newGrammar[A].push_back(symbols);
        }
    }

    for (const auto& [temp, rules] : intermediateRules) {
        for (const auto& rule : rules) {
            newGrammar[temp].push_back(rule);
        }
    }

    grammar = newGrammar;
}

    void convertTerminalsInRules() {
    std::unordered_map<Symbol, Symbol> termToVar;
    Grammar newGrammar;

    for (const auto& [A, prods] : grammar) {
        for (const auto& prod : prods) {
            if (prod.size() == 1 && !isNonTerminal(prod[0])) {
                newGrammar[A].push_back(prod);
                continue;
            }

            Production newProd;
            for (const auto& s : prod) {
                if (!isNonTerminal(s)) {
                    if (!termToVar.count(s)) {
                        Symbol newVar = getNewVariable();
                        termToVar[s] = newVar;
                        newGrammar[newVar].push_back({ s });
                    }
                    newProd.push_back(termToVar[s]);
                } else {
                    newProd.push_back(s);
                }
            }
            newGrammar[A].push_back(newProd);
        }
    }

    grammar = newGrammar;
    }

    bool isNonTerminal(const Symbol& s) {
        return !s.empty() && std::isupper(s[0]);
    }
};

int main() {
    CNFConverter::Grammar grammar = {
        {"S", {{"b", "A", "C"}, {"B"}}},
        {"A", {{"a"}, {"a", "S"}, {"b", "C", "a", "C", "b"}}},
        {"B", {{"A", "C"}, {"b", "S"}, {"a", "A", "a"}}},
        {"C", {{"~"}, {"A", "B"}}},
        {"E", {{"B", "A"}}}
    };

    CNFConverter converter(grammar, "S");
    CNFConverter::Grammar cnf = converter.convertToCNF();

    std::cout << "Converted CNF Grammar:\n";
    for (const auto& [nt, prods] : cnf) {
        for (const auto& p : prods) {
            std::cout << nt << " -> ";
            for (const auto& sym : p)
                std::cout << sym << " ";
            std::cout << "\n";
        }
    }

    return 0;
}
