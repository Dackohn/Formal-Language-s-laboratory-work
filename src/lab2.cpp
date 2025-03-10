#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iomanip>
#include <random>

// ======================== Grammar Class ========================
class Grammar {
private:
    std::set<std::string> VN; // Non-terminals (State names)
    std::set<char> VT; // Terminals (Alphabet)
    std::map<std::string, std::vector<std::string>> P; // Production rules
    std::string startSymbol;
    std::vector<std::string> generatedStrings;

public:
    // ✅ Constructor for manually defined grammar
    Grammar(const std::map<std::string, std::vector<std::string>>& rules) {
        P = rules;
        for (const auto& rule : rules) {
            VN.insert(rule.first);
        }
        startSymbol = P.begin()->first;
        for (const auto& rule : rules) {
            for (const auto& rhs : rule.second) {
                for (char c : rhs) {
                    if (isalpha(c) && !isupper(c)) { 
                        VT.insert(c);
                    }
                }
            }
        }
    }

    void classifyGrammar() {
        bool isRegular = true, isContextFree = true, isContextSensitive = true;
    
        for (const auto& rule : P) {
            std::string lhs = rule.first;
            
            for (const auto& rhs : rule.second) {
                if (rhs.empty()) continue; // Skip epsilon rules
                
                std::vector<std::string> rhsSymbols;
                std::string currentSymbol = "";
                
                // ✅ Properly split RHS into symbols (handling multi-character non-terminals)
                for (char c : rhs) {
                    if (c == ' ') { // Space means end of a symbol
                        if (!currentSymbol.empty()) {
                            rhsSymbols.push_back(currentSymbol);
                            currentSymbol = "";
                        }
                    } else {
                        currentSymbol += c;
                    }
                }
                if (!currentSymbol.empty()) rhsSymbols.push_back(currentSymbol);
    
                // ✅ Type 3 (Regular Grammar) Check
                if (rhsSymbols.size() > 2 || 
                    (rhsSymbols.size() == 2 && VN.find(rhsSymbols[1]) == VN.end())) {
                    isRegular = false;
                }
    
                // ✅ Type 2 (Context-Free Grammar) Check
                if (lhs.length() > 1 || VN.find(lhs) == VN.end()) {
                    isContextFree = false;
                }
    
                // ✅ Type 1 (Context-Sensitive Grammar) Check
                if (lhs.length() > rhs.length()) {
                    isContextSensitive = false;
                }
            }
        }
    
        std::cout << "\n====== Grammar Classification ======\n";
        if (isRegular)
            std::cout << "Type 3: Regular Grammar\n";
        else if (isContextFree)
            std::cout << "Type 2: Context-Free Grammar\n";
        else if (isContextSensitive)
            std::cout << "Type 1: Context-Sensitive Grammar\n";
        else
            std::cout << "Type 0: Unrestricted Grammar\n";
        std::cout << "====================================\n";
    }
    
    

    // ✅ Function to display grammar rules
    void displayGrammar() const {
        std::cout << "\n====== Regular Grammar ======\n";
        for (const auto& rule : P) {
            std::cout << rule.first << " -> ";
            for (size_t i = 0; i < rule.second.size(); i++) {
                std::cout << rule.second[i];
                if (i < rule.second.size() - 1) std::cout << " | ";
            }
            std::cout << std::endl;
        }
        std::cout << "============================\n";
    }

    std::string generateString() {
        std::string current = startSymbol;
        std::random_device rd;
        std::mt19937 gen(rd());
    
        int maxIterations = 100;  // Prevent infinite loops
        while (maxIterations--) {
            bool expanded = false;
            std::string newString;
            size_t i = 0;
    
            while (i < current.size()) {
                std::string symbol;
                size_t nextSpace = current.find(" ", i);
                if (nextSpace == std::string::npos) {
                    symbol = current.substr(i);
                    i = current.size();
                } else {
                    symbol = current.substr(i, nextSpace - i);
                    i = nextSpace + 1;
                }
    
                // ✅ DEBUG: Print the current symbol being processed
                std::cout << "Processing symbol: " << symbol << std::endl;
    
                // ✅ If symbol is a non-terminal, replace it
                if (VN.find(symbol) != VN.end() && !P[symbol].empty()) {
                    auto& rules = P[symbol];
    
                    // ✅ Prioritize terminal-only rules
                    std::vector<std::string> terminalRules;
                    std::vector<std::string> nonTerminalRules;
    
                    for (const auto& rule : rules) {
                        bool containsNonTerminal = false;
                        for (const auto& nt : VN) {
                            if (rule.find(nt) != std::string::npos) {
                                containsNonTerminal = true;
                                break;
                            }
                        }
                        if (containsNonTerminal)
                            nonTerminalRules.push_back(rule);
                        else
                            terminalRules.push_back(rule);
                    }
    
                    std::string replacement;
                    if (!terminalRules.empty()) {
                        // ✅ Prefer terminal rules if available
                        std::uniform_int_distribution<int> distrib(0, terminalRules.size() - 1);
                        replacement = terminalRules[distrib(gen)];
                    } else if (!nonTerminalRules.empty()) {
                        // ✅ If no terminal rule exists, pick any rule
                        std::uniform_int_distribution<int> distrib(0, nonTerminalRules.size() - 1);
                        replacement = nonTerminalRules[distrib(gen)];
                    } else {
                        replacement = "";  // Should not happen but prevents infinite loops
                    }
    
                    // ✅ Handle epsilon (`ε`) transition properly (delete non-terminal)
                    if (replacement.empty()) {
                        expanded = true;
                    } else {
                        newString += replacement + " ";
                        expanded = true;
                    }
                } else {
                    newString += symbol + " ";
                }
            }
    
            // ✅ DEBUG: Print new string after expansion step
            std::cout << "New String: " << newString << std::endl;
    
            // ✅ Stop if no expansions were made (only terminals remain)
            if (!expanded) break;
            current = newString;
        }
    
        generatedStrings.push_back(current);
        return current;
    }
    
    
    
    
    

    // ✅ Function to generate multiple strings
    void generateMultipleStrings(int count) {
        for (int i = 0; i < count; i++) {
            std::cout << "Generated String " << (i + 1) << ": " << generateString() << std::endl;
        }
    }

    // ✅ Function to retrieve generated strings
    const std::vector<std::string>& getGeneratedStrings() const {
        return generatedStrings;
    }
};

// ======================== Finite Automaton Class ========================
class FiniteAutomaton {
private:
    std::set<std::string> states;
    std::set<char> alphabet;
    std::map<std::pair<std::string, char>, std::set<std::string>> transitions;
    std::string startState;
    std::set<std::string> finalStates;

public:
    // ✅ Constructor
    FiniteAutomaton(const std::set<std::string>& states, 
                    const std::set<char>& alphabet, 
                    const std::map<std::pair<std::string, char>, std::set<std::string>>& transitions, 
                    const std::string& startState, 
                    const std::set<std::string>& finalStates) 
        : states(states), alphabet(alphabet), transitions(transitions), startState(startState), finalStates(finalStates) {}

        bool validateString(std::string input) {
            std::string currentState = startState;
        
            // ✅ Trim spaces from the input (handles trailing spaces)
            input.erase(0, input.find_first_not_of(" "));
            input.erase(input.find_last_not_of(" ") + 1);
        
            std::cout << "Validating String: \"" << input << "\" (Start at " << currentState << ")\n";
        
            for (char symbol : input) {
                if (symbol == ' ') continue;  // ✅ Ignore spaces
        
                std::cout << "Current State: " << currentState << ", Reading: " << symbol << "\n";
        
                // ✅ Clean state (remove `{}` from multi-state names)
                std::string cleanedState = currentState;
                if (cleanedState[0] == '{' && cleanedState.back() == '}') {
                    cleanedState = cleanedState.substr(1, cleanedState.size() - 2);
                }
        
                // ✅ Handle multiple states in DFA (state sets)
                std::set<std::string> currentStates;
                size_t pos = 0;
                while ((pos = cleanedState.find(",")) != std::string::npos) {
                    currentStates.insert(cleanedState.substr(0, pos));
                    cleanedState.erase(0, pos + 1);
                }
                currentStates.insert(cleanedState);  // Add last state
        
                std::set<std::string> nextStates;
                for (const auto& state : currentStates) {
                    if (transitions.find({state, symbol}) != transitions.end()) {
                        nextStates.insert(transitions[{state, symbol}].begin(), transitions[{state, symbol}].end());
                    }
                }
        
                if (!nextStates.empty()) {
                    currentState = "{" + joinStates(nextStates) + "}";  // Convert back to DFA state format
                    std::cout << "Moved to: " << currentState << "\n";
                } else {
                    std::cout << "Transition Not Found! String is Invalid.\n";
                    return false;  // No valid transition found
                }
            }
        
            // ✅ Final validation step
            std::string cleanedFinalState = currentState;
            if (cleanedFinalState[0] == '{' && cleanedFinalState.back() == '}') {
                cleanedFinalState = cleanedFinalState.substr(1, cleanedFinalState.size() - 2);
            }
        
            std::set<std::string> finalStateSet;
            size_t pos = 0;
            while ((pos = cleanedFinalState.find(",")) != std::string::npos) {
                finalStateSet.insert(cleanedFinalState.substr(0, pos));
                cleanedFinalState.erase(0, pos + 1);
            }
            finalStateSet.insert(cleanedFinalState);  // Add last state
        
            for (const auto& state : finalStateSet) {
                if (finalStates.find(state) != finalStates.end()) {
                    std::cout << "Final State Reached: " << state << " (String is VALID!)\n";
                    return true;
                }
            }
        
            std::cout << "Final State " << currentState << " is NOT in Final States! (String is INVALID!)\n";
            return false;
        }
        

    Grammar convertToGrammar() {
        std::map<std::string, std::vector<std::string>> rules;
    
        for (const auto& transition : transitions) {
            std::string fromState = transition.first.first;
            char inputSymbol = transition.first.second;
    
            for (const std::string& toState : transition.second) {
                if (finalStates.count(toState)) {
                    rules[fromState].push_back(std::string(1, inputSymbol));
                }
                rules[fromState].push_back(std::string(1, inputSymbol) + " " + toState);
            }
        }
        for (const std::string& finalState : finalStates) {
            rules[finalState].push_back("");
        }
    
        return Grammar(rules);
    }
    

    
    FiniteAutomaton convertToDFA() {
        std::map<std::set<std::string>, std::string> stateMapping;
        std::vector<std::set<std::string>> stateQueue = {{startState}};
        std::map<std::pair<std::string, char>, std::set<std::string>> newTransitions;
        std::set<std::string> newStates;
        std::set<std::string> newFinalStates;
        std::string newStartState = "{" + startState + "}";
    
        stateMapping[{startState}] = newStartState;
    
        while (!stateQueue.empty()) {
            std::set<std::string> currentSet = stateQueue.back();
            stateQueue.pop_back();
    
            std::string currentDFAState = (currentSet.size() == 1) ? *currentSet.begin() : "{" + joinStates(currentSet) + "}";
            newStates.insert(currentDFAState);
    
            for (char symbol : alphabet) {
                std::set<std::string> newState;
                for (const std::string& state : currentSet) {
                    if (transitions.find({state, symbol}) != transitions.end()) {
                        newState.insert(transitions[{state, symbol}].begin(), transitions[{state, symbol}].end());
                    }
                }
    
                if (!newState.empty()) {
                    std::string newStateName = (newState.size() == 1) ? *newState.begin() : "{" + joinStates(newState) + "}";
                    if (stateMapping.find(newState) == stateMapping.end()) {
                        stateMapping[newState] = newStateName;
                        stateQueue.push_back(newState);
                    }
                    newTransitions[{currentDFAState, symbol}] = {newStateName};
                }
            }
    
            for (const std::string& state : currentSet) {
                if (finalStates.count(state)) {
                    newFinalStates.insert(currentDFAState);
                }
            }
        }
    
        return FiniteAutomaton(newStates, alphabet, newTransitions, newStartState, newFinalStates);
    }

    
    void printDFA() {
        std::cout << "\n====== Deterministic Finite Automaton (DFA) ======\n";
    
        std::cout << "States: { ";
        for (const auto& state : states) {
            std::cout << state << " ";
        }
        std::cout << "}\n";
    
        std::cout << "Alphabet: { ";
        for (char symbol : alphabet) {
            std::cout << symbol << " ";
        }
        std::cout << "}\n";
    
        std::cout << "\nTransition Table:\n";
        std::cout << std::setw(15) << "State" << std::setw(10) << "Input" << std::setw(20) << "Next State(s)\n";
        std::cout << "-------------------------------------------------\n";
    
        for (const auto& transition : transitions) {
            std::cout << std::setw(15) << transition.first.first << " "
                      << std::setw(5) << transition.first.second << "  -> { ";
    
            for (const auto& nextState : transition.second) {
                std::cout << nextState << " ";
            }
            std::cout << "}\n";
        }
    
        std::cout << "\nStart State: " << startState << "\n";
    
        std::cout << "Final States: { ";
        for (const auto& finalState : finalStates) {
            std::cout << finalState << " ";
        }
        std::cout << "}\n";
    
        std::cout << "==========================================\n";
    }

    
    std::string joinStates(const std::set<std::string>& stateSet) {
        if (stateSet.empty()) return "{}";
        std::string result;
        for (const auto& state : stateSet) {
            if (!result.empty()) result += ",";
            result += state;
        }
        return result;
    }
    
};

int main() {
    // ✅ Define NDFA for Variant 23
    std::set<std::string> states = {"q0", "q1", "q2"};
    std::set<char> alphabet = {'a', 'b'};
    std::map<std::pair<std::string, char>, std::set<std::string>> transitions = {
        {{"q0", 'a'}, {"q0", "q1"}},  // δ(q0, a) → q0, q1
        {{"q1", 'b'}, {"q2"}},        // δ(q1, b) → q2
        {{"q0", 'b'}, {"q0"}},        // δ(q0, b) → q0
        {{"q2", 'b'}, {"q2"}},        // δ(q2, b) → q2
        {{"q1", 'a'}, {"q0"}}         // δ(q1, a) → q0
    };
    std::string startState = "q0";
    std::set<std::string> finalStates = {"q2"};

    // ✅ Create NDFA and Convert to DFA
    FiniteAutomaton nfa(states, alphabet, transitions, startState, finalStates);
    FiniteAutomaton dfa = nfa.convertToDFA();

    // ✅ Print DFA Transition Table
    std::cout << "\n===== DFA Transition Table =====\n";
    dfa.printDFA();

    // ✅ Convert DFA to Grammar
    Grammar grammar = dfa.convertToGrammar();
    
    // ✅ Display the Generated Regular Grammar
    std::cout << "\n===== Generated Regular Grammar =====\n";
    grammar.displayGrammar();

    // ✅ Classify Grammar
    grammar.classifyGrammar();

    // ✅ Generate Strings from Grammar
    std::cout << "\n===== Testing String Generation =====\n";
    grammar.generateMultipleStrings(5);

    // ✅ Validate Strings with DFA
    std::cout << "\n===== Validating Generated Strings =====\n";
    for (const std::string& str : grammar.getGeneratedStrings()) {
        std::cout << "String: " << str << " -> " 
                  << (dfa.validateString(str) ? "Valid" : "Invalid") << "\n";
    }

    return 0;
}
