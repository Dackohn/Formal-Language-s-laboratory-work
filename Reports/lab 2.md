### **Determinism in Finite Automata: Conversion from NDFA to DFA and Chomsky Hierarchy**
### **Student : Pleșca Denis**
### **Teacher : Crețu Dumitru**

## **1. Introduction**
Finite automata are abstract machines used in theoretical computer science to recognize patterns in strings. They are classified into **Deterministic Finite Automata (DFA)** and **Non-Deterministic Finite Automata (NDFA)**. This report explores determinism in finite automata, the conversion from an NDFA to a DFA, and classification based on the **Chomsky hierarchy**.

## **2. Theory**

### **2.1 Deterministic vs. Non-Deterministic Finite Automata**
A **Deterministic Finite Automaton (DFA)** is a state machine where each state has exactly one transition for each input symbol. In contrast, a **Non-Deterministic Finite Automaton (NDFA)** allows multiple transitions for a single symbol or transitions without consuming an input symbol (ε-transitions).

An NDFA can be converted into an equivalent DFA using the **subset construction algorithm**, ensuring that both recognize the same language.

### **Subset Construction Algorithm for NDFA to DFA Conversion**

The **Subset Construction Algorithm**, also known as the **Powerset Construction**, is used to convert a **Non-Deterministic Finite Automaton (NDFA)** into an equivalent **Deterministic Finite Automaton (DFA)**. Since an NDFA allows multiple possible transitions for a given input, this algorithm groups sets of NDFA states into single DFA states, ensuring that the resulting DFA has deterministic transitions.

#### **Algorithm Overview**

Given an NDFA defined as a tuple **(Q, Σ, δ, q0, F)**, where:
- **Q**: A finite set of states.
- **Σ**: A finite input alphabet.
- **δ**: A transition function **(Q × Σ → P(Q))** mapping states and input symbols to sets of states.
- **q0**: The initial state.
- **F**: The set of final (accepting) states.

The algorithm constructs a DFA **(Q', Σ, δ', q0', F')** as follows:

1. **Initialize** the DFA:
   - The start state of the DFA **q0'** is the **epsilon-closure** of the NDFA start state **q0**.
   - The DFA state set **Q'** starts as an empty set.
   - The transition function **δ'** starts empty.

2. **Construct DFA states**:
   - Use a queue to track unprocessed sets of NDFA states.
   - Assign each unique set of NDFA states a new DFA state.
   - Process each DFA state by determining its transitions for each input symbol **a ∈ Σ**:
     - Compute the **epsilon-closure** of the resulting state set.
     - If this state set is new, add it to the DFA state set **Q'** and queue it for processing.

3. **Determine Final States**:
   - Any DFA state that contains an NDFA final state is a DFA final state.

4. **Build the DFA Transition Table**:
   - For each DFA state and input symbol, define a deterministic transition to a unique DFA state.

#### **Example of Subset Construction**

##### **Given NDFA:**

| State | Input 'a' | Input 'b' |
|--------|----------|----------|
| q0     | {q0, q1} | {}       |
| q1     | {}       | {q2}     |
| q2     | {}       | {q2}     |

##### **Conversion to DFA:**
1. **Start State**: `{q0}`
2. **Compute Transitions**:
   - `{q0} --a--> {q0, q1}`
   - `{q0} --b--> {}` (Dead state)
   - `{q0, q1} --a--> {q0}`
   - `{q0, q1} --b--> {q2}`
   - `{q2} --b--> {q2}`

3. **Final DFA Transition Table:**

| DFA State | Input 'a' | Input 'b' |
|-----------|----------|----------|
| {q0}      | {q0, q1} | ∅ |
| {q0, q1}  | {q0}    | {q2} |
| {q2}      | ∅     | {q2} |

#### **Time Complexity**
- The worst-case complexity of subset construction is **O(2^n)**, where **n** is the number of NDFA states, as the DFA can have up to **2^n** states.

##### **2.2 Chomsky Hierarchy**
Chomsky hierarchy classifies grammars into four types:
- **Type 0 (Unrestricted Grammars)**: Most powerful, capable of describing recursively enumerable languages.
- **Type 1 (Context-Sensitive Grammars)**: Requires that production rules maintain or increase string length.
- **Type 2 (Context-Free Grammars - CFGs)**: Used in programming languages; rules have a single non-terminal on the left-hand side.
- **Type 3 (Regular Grammars)**: Equivalent to finite automata, used in lexical analysis.

#### **3. Objectives**
1. Understand the concept of determinism in finite automata.
2. Implement an NDFA and convert it to an equivalent DFA.
3. Classify a given grammar based on the Chomsky hierarchy.

#### **4. Implementation**


### **4.2 Finite Automaton Definition**
The `FiniteAutomaton` class represents a finite automaton with transitions, states, and a validation function.

```cpp
class FiniteAutomaton {
private:
    std::set<std::string> states;  // Set of states
    std::set<char> alphabet;  // Input alphabet
    std::map<std::pair<std::string, char>, std::set<std::string>> transitions;  // State transitions
    std::string startState;  // Start state
    std::set<std::string> finalStates;  // Final (accepting) states

public:
    // Constructor initializing automaton properties
    FiniteAutomaton(std::set<std::string> st, std::set<char> alph, std::map<std::pair<std::string, char>, std::set<std::string>> trans, std::string start, std::set<std::string> finals) 
        : states(st), alphabet(alph), transitions(trans), startState(start), finalStates(finals) {}
};
```
The `FiniteAutomaton` class encapsulates the representation of a finite automaton, which consists of several key components:
- **states**: A set that holds all the states in the automaton.
- **alphabet**: A set containing the input symbols (characters) that the automaton can process.
- **transitions**: A map that associates pairs of states and input symbols to a set of destination states. This defines how the automaton transitions between states based on the input symbols.
- **startState**: A string representing the start state, which is where the automaton begins its computation.
- **finalStates**: A set of strings representing the final (accepting) states, where if the automaton ends, it will accept the input string.

The constructor initializes these components using the parameters provided, enabling the creation of a finite automaton with specific states, transitions, and alphabet.


### **4.3 Checking Determinism**
```cpp
bool IsDeterministic() {
        for (const auto& [state, transitions] : Transitions) {
            for (const auto& [symbol, targets] : transitions) {
                if (targets.size() > 1) return false; // Multiple transitions for same symbol
            }
        }
        return true;
    }
```
This function determines whether the automaton is deterministic or not:
- It iterates over all transitions stored in the `transitions` map.
- For each state-symbol pair, it checks if there is more than one next state in the corresponding set of destination states.
- If any transition results in multiple possible next states for the same state and input symbol, it indicates that the automaton is **non-deterministic**.
- If all transitions are deterministic (i.e., there is only one state for each state-symbol pair), the automaton is considered **deterministic**.

This function returns `true` if the automaton is deterministic, and `false` if it is non-deterministic.


### **4.4 Converting NDFA to DFA**
```cpp
FiniteAutomaton ConvertToDFA() {
        if (IsDeterministic()) return *this;

        std::unordered_set<State> newStates;
        TransitionMap newTransitions;
        std::unordered_set<State> newFinalStates;
        std::queue<std::unordered_set<State>> queue;
        std::unordered_map<std::unordered_set<State>, State, HashSetComparer> stateMapping;

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

```
This function converts a Non-Deterministic Finite Automaton (NDFA) into a Deterministic Finite Automaton (DFA) using the **Subset Construction Algorithm** (also known as Powerset Construction). The steps involved are:

1. **Initial Setup**:  
   - The function begins by checking if the NDFA is already deterministic; if so, it returns the automaton as is.
   - It initializes a set of new states (`newStates`), a map of transitions (`newTransitions`), and a set of final states (`newFinalStates`).
   - A queue is used to manage the unprocessed sets of states, starting with the epsilon closure of the start state.

2. **State Mapping**:  
   - For each set of NDFA states (represented as `currentSet`), it generates a new state in the DFA by mapping the set to a unique DFA state.
   - The transitions between DFA states are calculated based on the transitions of the NDFA. Each possible input symbol is processed to determine where the automaton should go, and the corresponding new state is added to the DFA.

3. **Final States**:  
   - A DFA state is marked as a final state if any of the NDFA states in that set is a final state.

4. **Transition Mapping**:  
   - The transitions are defined by mapping the current DFA state and input symbol to the appropriate next state in the new DFA.

5. **Breadth-First Search (BFS)**:  
   - The queue ensures that each set of NDFA states is processed and transitioned to a new DFA state in a breadth-first manner.
   - The function iterates over the NDFA states and transitions, grouping them into sets and assigning them new DFA states.

The final output is a DFA that accepts the same language as the original NDFA, with deterministic transitions and final states.



```cpp
Grammar ToGrammar() {
        std::unordered_map<State, std::string> stateToNonTerminal;
        std::unordered_set<std::string> nonTerminals;
        std::unordered_set<std::string> terminals(Alphabet);
        std::unordered_map<std::string, std::vector<std::string>> productions;

        // Create a mapping of states to non-terminals (A, B, C, ...)
        char nonTerminalLetter = 'A';  // Start from 'A'
        for (const auto& state : States) {
            std::string nonTerminal(1, nonTerminalLetter++);
            stateToNonTerminal[state] = nonTerminal;
            nonTerminals.insert(nonTerminal);

            // Print the transformation
            std::cout << state << " -> " << nonTerminal << std::endl;
        }

        // Create production rules based on the transitions
        for (const auto& [state, transitions] : Transitions) {
            std::string nonTerminal = stateToNonTerminal[state];
            for (const auto& [symbol, targets] : transitions) {
                for (const auto& target : targets) {
                    std::string targetNonTerminal = stateToNonTerminal[target];
                    productions[nonTerminal].push_back(symbol + targetNonTerminal);
                    if (FinalStates.count(target)) {
                        productions[nonTerminal].push_back(symbol); // A -> a (terminal)
                    }
                }
            }
        }

        return Grammar(nonTerminals, terminals, productions, stateToNonTerminal[StartState]);
    }

```
This function converts a finite automaton (NDFA or DFA) into a grammar, following the standard procedure for converting an automaton to a context-free grammar (CFG):

1. **State to Non-Terminal Mapping**:  
   - Each state of the automaton is mapped to a unique non-terminal symbol. The non-terminals are created sequentially starting from 'A', 'B', etc., and are stored in a map `stateToNonTerminal`.
   - A set of non-terminals is generated based on the states of the automaton.

2. **Production Rules**:  
   - The transitions of the automaton are used to create production rules for the grammar. For each state, and for each transition (symbol), the corresponding non-terminal symbol for the destination state is added to the production.
   - If a state has a transition to a final state, an additional production rule is added that represents the terminal symbol, since a final state indicates acceptance of the input string.

3. **Final Grammar Construction**:  
   - The grammar includes a set of non-terminals, terminals (input alphabet), and production rules that correspond to the transitions in the automaton.
   - The start symbol of the grammar is the non-terminal corresponding to the automaton's start state.

The output is a **context-free grammar (CFG)** equivalent to the automaton, which generates the same language as the automaton.



**Description:**
- Uses the **subset construction method** to convert NDFA to DFA.
- Groups multiple states into single DFA states.
- Uses a queue for **Breadth-First Search (BFS)** to traverse the NDFA.

## **5. Conclusion**  

This report explored the fundamental concepts of finite automata, focusing on **determinism**, **NDFA to DFA conversion**, and **Chomsky hierarchy classification**. The **subset construction algorithm** was analyzed and implemented, demonstrating how an NDFA can be systematically transformed into a DFA by grouping multiple states into single deterministic states.

The implementation provided insight into the theoretical and practical aspects of automata theory, emphasizing how finite automata serve as the foundation for pattern recognition and language processing. The analysis of **time complexity** revealed that while DFA conversion can lead to an exponential increase in the number of states, it is essential for deterministic computation.

Furthermore, the transformation of **finite automata into grammars** illustrated the strong connections between automata theory and formal language theory, showcasing how regular languages can be represented both as state machines and production-based systems.

Future improvements could focus on **state minimization techniques** to optimize the DFA, reducing redundant states while preserving language equivalence. Additionally, integrating **Graphviz-based visualization** of automata would enhance the understanding of automaton transitions and behaviors.

Overall, this study reinforces the significance of automata in theoretical computer science and its applications in **compilers, lexical analysis, and artificial intelligence**.


