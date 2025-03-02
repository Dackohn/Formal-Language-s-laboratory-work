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
**Description:**
- Defines **states**, **input symbols**, **transition rules**, **start state**, and **final states**.
- Stores transition rules using a **map of state-symbol pairs**.

### **4.3 Checking Determinism**
```cpp
bool isDeterministic() {
    for (const auto &[key, nextStates] : transitions) {
        if (nextStates.size() > 1) { // If multiple states for one transition
            return false;
        }
    }
    return true;
}
```
**Description:**
- Iterates through transitions and checks if any transition leads to multiple states.
- If any transition has more than one next state, the automaton is **non-deterministic**.

### **4.4 Converting NDFA to DFA**
```cpp
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
                stateMapping[nextSet] = newName;
                newStates.insert(newName);
                newTransitions[{newStateName, symbol}] = newName;
            }
        }
    }
    return FiniteAutomaton(newStates, alphabet, newTransitions, "{" + startState + "}", finalStates);
}
```
**Description:**
- Uses the **subset construction method** to convert NDFA to DFA.
- Groups multiple states into single DFA states.
- Uses a queue for **Breadth-First Search (BFS)** to traverse the NDFA.

## **5. Conclusion**
This report covers the **determinism in finite automata**, **conversion from NDFA to DFA**, and **Chomsky hierarchy** classification. The implementation provides a systematic approach to understanding finite automata and their transformations.

