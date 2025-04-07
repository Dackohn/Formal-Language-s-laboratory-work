#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>

class FiniteAutomaton {
public:
    using State = std::string;
    using Symbol = std::string;
    using TransitionMap = std::unordered_map<State, std::unordered_map<Symbol, std::unordered_set<State>>>;

    std::unordered_set<State> States;
    std::unordered_set<Symbol> Alphabet;
    TransitionMap Transitions;
    State StartState;
    std::unordered_set<State> FinalStates;

    FiniteAutomaton(
        const std::unordered_set<State>& states,
        const std::unordered_set<Symbol>& alphabet,
        const TransitionMap& transitions,
        const State& startState,
        const std::unordered_set<State>& finalStates)
        : States(states), Alphabet(alphabet), Transitions(transitions), StartState(startState), FinalStates(finalStates) {}

    bool StringBelongsToLanguage(const std::string& inputString) {
        std::unordered_set<State> currentStates = { StartState };

        for (char c : inputString) {
            std::unordered_set<State> nextStates;
            std::string symbol(1, c);
            for (const auto& state : currentStates) {
                if (Transitions.count(state) && Transitions[state].count(symbol)) {
                    nextStates.insert(Transitions[state][symbol].begin(), Transitions[state][symbol].end());
                }
            }

            if (nextStates.empty()) return false; // No valid transition
            currentStates = nextStates;
        }

        for (const auto& state : currentStates) {
            if (FinalStates.count(state)) return true;
        }
        return false;
    }

    bool IsDeterministic() {
        for (const auto& [state, transitions] : Transitions) {
            for (const auto& [symbol, targets] : transitions) {
                if (targets.size() > 1) return false; // Multiple transitions for same symbol
            }
        }
        return true;
    }

    FiniteAutomaton ConvertToDFA() {
        if (IsDeterministic()) return *this;

        std::unordered_set<State> newStates;
        TransitionMap newTransitions;
        std::unordered_set<State> newFinalStates;
        std::queue<std::unordered_set<State>> queue;
        std::unordered_map<std::unordered_set<State>, State> stateMapping;

        std::unordered_set<State> startSet = { StartState };
        queue.push(startSet);
        stateMapping[startSet] = JoinStates(startSet);
        newStates.insert(stateMapping[startSet]);

        if (ContainsFinalState(startSet)) newFinalStates.insert(stateMapping[startSet]);

        while (!queue.empty()) {
            auto currentSet = queue.front();
            queue.pop();
            State currentState = stateMapping[currentSet];

            for (const auto& symbol : Alphabet) {
                std::unordered_set<State> nextSet;
                for (const auto& state : currentSet) {
                    if (Transitions.count(state) && Transitions[state].count(symbol)) {
                        nextSet.insert(Transitions[state][symbol].begin(), Transitions[state][symbol].end());
                    }
                }

                if (!nextSet.empty() && !stateMapping.count(nextSet)) {
                    stateMapping[nextSet] = JoinStates(nextSet);
                    queue.push(nextSet);
                    newStates.insert(stateMapping[nextSet]);
                    if (ContainsFinalState(nextSet)) newFinalStates.insert(stateMapping[nextSet]);
                }
                
                newTransitions[currentState][symbol] = { stateMapping[nextSet] };
            }
        }

        return FiniteAutomaton(newStates, Alphabet, newTransitions, stateMapping[startSet], newFinalStates);
    }

    std::string ToDot() {
        std::ostringstream sb;
        sb << "digraph DFA {\n";
        sb << "  rankdir=LR;\n  node [shape=circle];\n";

        for (const auto& finalState : FinalStates) {
            sb << "  \"" << finalState << "\" [shape=doublecircle];\n";
        }

        sb << "  \"\" -> \"" << StartState << "\" [label=\"start\"];\n";

        for (const auto& [state, transitions] : Transitions) {
            for (const auto& [symbol, targets] : transitions) {
                for (const auto& target : targets) {
                    sb << "  \"" << state << "\" -> \"" << target << "\" [label=\"" << symbol << "\"];\n";
                }
            }
        }

        sb << "}";
        return sb.str();
    }

private:
    static std::string JoinStates(const std::unordered_set<State>& states) {
        std::string result;
        for (const auto& state : states) {
            result += state;
        }
        return result;
    }

    bool ContainsFinalState(const std::unordered_set<State>& states) {
        for (const auto& state : states) {
            if (FinalStates.count(state)) return true;
        }
        return false;
    }
};