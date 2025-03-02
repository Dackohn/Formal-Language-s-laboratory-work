#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <random>
#include <queue>
#include <fstream>

class Grammar {
private:
    std::set<char> VN;
    std::set<char> VT;
    std::map<char, std::vector<std::string>> P;
    char startSymbol;
    std::vector<std::string> generatedStrings;

public:
    Grammar(std::set<char> vn, std::set<char> vt, std::map<char, std::vector<std::string>> p, char start) 
        : VN(vn), VT(vt), P(p), startSymbol(start) {}

    std::string generateString() {
        std::string current = std::string(1, startSymbol);
        std::random_device rd;
        std::mt19937 gen(rd());

        while (true) {
            bool expanded = false;
            for (size_t i = 0; i < current.size(); ++i) {
                char c = current[i];
                if (VN.find(c) != VN.end()) {
                    auto &rules = P[c];
                    std::uniform_int_distribution<> distrib(0, rules.size() - 1);
                    std::string replacement = rules[distrib(gen)];

                    current.replace(i, 1, replacement);
                    expanded = true;
                    break;
                }
            }
            if (!expanded)
                break;
        }
        generatedStrings.push_back(current);
        return current;
    }

    void generateMultipleStrings(int count) {
        for (int i = 0; i < count; i++) {
            std::cout << "Generated String " << (i + 1) << ": " << generateString() << std::endl;
        }
    }

    bool validateString(const std::string &input) {
        for (const auto &generated : generatedStrings) {
            if (generated == input) {
                return true;
            }
        }
        return false;
    }

    const std::vector<std::string> &getGeneratedStrings() const {
        return generatedStrings;
    }
};

class FiniteAutomaton {
private:
    std::set<std::string> states;
    std::set<char> alphabet;
    std::map<std::pair<std::string, char>,std::set<std::string>> transitions;
    std::string startState;
    std::set<std::string> finalStates;

public:
    FiniteAutomaton(std::set<std::string> st, std::set<char> alph, std::map<std::pair<std::string, char>, std::set<std::string>> trans, std::string start, std::set<std::string> finals) 
        : states(st), alphabet(alph), transitions(trans), startState(start), finalStates(finals) {}

    bool isDeterministic() {
        for (const auto &[key, nextStates] : transitions) {
            if (nextStates.size() > 1) {
                return false; 
            }
        }
        return true;
    }

    FiniteAutomaton convertToDFA() {
        std::map<std::set<std::string>, std::string> stateMapping;
        std::set<std::string> newStates;
        std::map<std::pair<std::string, char>, std::string> newTransitions;
        std::queue<std::set<std::string>> stateQueue;
        
        std::set<std::string> startSet = {startState};
        stateQueue.push(startSet);
        stateMapping[startSet] = "{" + startState + "}";
        newStates.insert("{" + startState + "}");
        
        while (!stateQueue.empty()) {
            std::set<std::string> currentSet = stateQueue.front();
            stateQueue.pop();
            std::string newStateName = stateMapping[currentSet];
            
            for (char symbol : alphabet) {
                std::set<std::string> nextSet;
                for (const std::string &state : currentSet) {
                    if (transitions.count({state, symbol})) {
                        nextSet.insert(transitions[{state, symbol}].begin(), transitions[{state, symbol}].end());
                    }
                }
                
                if (!nextSet.empty()) {
                    std::string newName = "{";
                    for (const auto &s : nextSet) {
                        newName += s + ",";
                    }
                    newName.pop_back(); 
                    newName += "}";
                    
                    if (stateMapping.find(nextSet) == stateMapping.end()) {
                        stateMapping[nextSet] = newName;
                        newStates.insert(newName);
                        stateQueue.push(nextSet);
                    }
                    newTransitions[{newStateName, symbol}] = stateMapping[nextSet];
                }
            }
        }
        
        std::set<std::string> newFinalStates;
        for (const auto &[subset, mappedName] : stateMapping) {
            for (const auto &s : subset) {
                if (finalStates.find(s) != finalStates.end()) {
                    newFinalStates.insert(mappedName);
                    break;
                }
            }
        }
        
        return FiniteAutomaton(newStates, alphabet, transitions, "{" + startState + "}", newFinalStates);
    }

    void printDFA() {
        std::cout << "\nConverted DFA:" << std::endl;
        std::cout << "States: ";
        for (const auto &state : states) {
            std::cout << state << " ";
        }
        std::cout << "\nAlphabet: ";
        for (char symbol : alphabet) {
            std::cout << symbol << " ";
        }
        std::cout << "\nStart State: " << startState << "\n";
        std::cout << "Final States: ";
        for (const auto &state : finalStates) {
            std::cout << state << " ";
        }
        std::cout << "\nTransitions:" << std::endl;
        
        for (const auto &[key, nextStates] : transitions) {
            std::cout << key.first << " --" << key.second << "--> {";
            bool first = true;
            for (const auto &nextState : nextStates) {
                if (!first) std::cout << ", ";
                std::cout << nextState;
                first = false;
            }
            std::cout << "}" << std::endl;
        }
    }
    

    void visualizeAutomaton() {
        std::ofstream file("automaton.dot");
        file << "digraph FiniteAutomaton {\n";
        file << "    rankdir=LR;\n";
        file << "    node [shape = doublecircle];";
        for (const std::string &state : finalStates) {
            file << " " << state;
        }
        file << ";\n    node [shape = circle];\n";
    
        for (const auto &[key, nextStates] : transitions) {
            for (const std::string &nextState : nextStates) {
                file << "    " << key.first << " -> " << nextState << " [label = \"" << key.second << "\"];\n";
            }
        }
        file << "}";
        file.close();
        std::cout << "Graph representation saved as automaton.dot (Use Graphviz to visualize).\n";
    }
    
    
};

int main() {
    std::set<std::string> states = {"q0", "q1", "q2"};
    std::set<char> alphabet = {'a', 'b'};
    std::map<std::pair<std::string, char>,std::set<std::string>> transitions = {
        {{"q0", 'a'}, {"q0", "q1"}},
        {{"q1", 'b'}, {"q2"}},
        {{"q0", 'b'}, {"q0"}},
        {{"q2", 'b'}, {"q2"}},
        {{"q1", 'a'}, {"q0"}}
    };
    std::string startState = "q0";
    std::set<std::string> finalStates = {"q2"};

    FiniteAutomaton fa(states, alphabet, transitions, startState, finalStates);

    if (!fa.isDeterministic()) {
        std::cout << "The Finite Automaton is Non-Deterministic. Converting to DFA...\n";
        fa = fa.convertToDFA();
        fa.printDFA();
    }

    std::cout << "\nVisualizing Finite Automaton...\n";
    fa.visualizeAutomaton();

    std::cout << "\nFinal Automaton is Deterministic." << std::endl;
    return 0;
}
