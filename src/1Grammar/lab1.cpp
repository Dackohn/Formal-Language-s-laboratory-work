#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <random>
//V23
class Grammar {
private:
    std::set<char> VN = {'S', 'B', 'C'};
    std::set<char> VT = {'a', 'b', 'c'}; 
    std::map<char, std::vector<std::string>> P;
    char startSymbol = 'S';
    std::vector<std::string> generatedStrings;

public:
    // Initialize the grammar rules
    Grammar() {
        P['S'] = {"aB"};
        P['B'] = {"aC", "bB"};
        P['C'] = {"bB", "c", "aS"};
    }

    std::string generateString() {
        std::string current = std::string(1, startSymbol);
        std::random_device rd;
        std::mt19937 gen(rd());

        while (true) {
            bool expanded = false;
            for (size_t i = 0; i < current.size(); ++i) {
                char c = current[i];
                if (VN.find(c) != VN.end()) {  // If it's a non-terminal
                    auto& rules = P[c];
                    std::uniform_int_distribution<> distrib(0, rules.size() - 1);
                    std::string replacement = rules[distrib(gen)];

                    current.replace(i, 1, replacement);
                    expanded = true;
                    break;
                }
            }
            if (!expanded) break;
        }
        generatedStrings.push_back(current);
        return current;
    }

    void generateMultipleStrings(int count) {
        for (int i = 0; i < count; i++) {
            std::cout << "Generated String " << (i + 1) << ": " << generateString() << std::endl;
        }
    }

    const std::vector<std::string>& getGeneratedStrings() const {
        return generatedStrings;
    }
};

//................................................//
class FiniteAutomaton {
private:
    std::set<char> states = {'S', 'B', 'C'};  // Predefined states
    std::set<char> alphabet = {'a', 'b', 'c'};  // Alphabet
    std::map<std::pair<char, char>, char> transitions;
    char startState = 'S';
    std::set<char> finalStates = {'C'};  

public:
    // Constructor initializes the transitions based on the grammar rules
    FiniteAutomaton() {
        transitions[{'S', 'a'}] = 'B';
        transitions[{'B', 'a'}] = 'C';
        transitions[{'B', 'b'}] = 'B'; 
        transitions[{'C', 'b'}] = 'B'; 
        transitions[{'C', 'c'}] = 'C';
        transitions[{'C', 'a'}] = 'S';  
    }
       
    bool validateString(const std::string& input) {
        char currentState = startState;

        for (char symbol : input) {
            if (transitions.find({currentState, symbol}) != transitions.end()) {
                currentState = transitions[{currentState, symbol}];
            } else {
                return false;
            }
        }
        return finalStates.find(currentState) != finalStates.end();
    }
};


int main() {
    Grammar grammar;

    std::cout << "Generated Strings from Grammar:\n";
    grammar.generateMultipleStrings(5);

    FiniteAutomaton fa;


    std::cout << "\nTesting Strings in Finite Automaton:\n";
    const auto& testStrings = grammar.getGeneratedStrings();
    for (const auto& str : testStrings) {
        if (fa.validateString(str)) {
            std::cout << "String '" << str << "' is VALID.\n";
        } else {
            std::cout << "String '" << str << "' is INVALID.\n";
        }
    }

    return 0;
}
