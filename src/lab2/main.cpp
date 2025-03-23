#include "FiniteAutomaton.hpp"
#include <iostream>

int main() {
    // Define NDFA Variant 23
    std::unordered_set<std::string> states = {"q0", "q1", "q2"};
    std::unordered_set<std::string> alphabet = {"a", "b"};
    std::unordered_set<std::string> finalStates = {"q2"};
    std::string startState = "q0";
    
    FiniteAutomaton::TransitionMap transitions = {
        {"q0", {{"a", {"q0", "q1"}}, {"b", {"q0"}}}},
        {"q1", {{"b", {"q2"}}, {"a", {"q0"}}}},
        {"q2", {{"b", {"q2"}}}}
    };

    FiniteAutomaton ndfa(states, alphabet, transitions, startState, finalStates);

    std::cout << "The automaton is " << (ndfa.IsDeterministic() ? "deterministic" : "non-deterministic") << "\n";

    FiniteAutomaton dfa = ndfa.ConvertToDFA();
    dfa.PrintDFA();

    Grammar grammar = ndfa.ToGrammar();
    std::cout << "Grammar classification: " << grammar.ClassifyGrammar() << "\n";

    std::cout << "Generated strings: \n";
    auto generatedStrings = grammar.GenerateStrings();
    for (const auto& str : generatedStrings) {
        std::cout << str << " -> " << (dfa.StringBelongsToLanguage(str) ? "Valid" : "Invalid") << "\n";
    }

    dfa.ToDot();
    ndfa.ToDot("ndfa.dot");
    return 0;
}