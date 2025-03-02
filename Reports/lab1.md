# 1_RegularGrammars

### Course: Formal Languages & Finite Automata
### Author: Pleșca Denis

----

## Theory

### **1. Formal Language and Grammar**

A **formal language** defines valid sequences of symbols using an alphabet and production rules. These rules determine how one sequence of symbols transforms into another. Regular grammars, the simplest type, follow strict patterns that can be recognized by finite automata. A grammar consists of **non-terminal symbols**, **terminal symbols**, **production rules**, and **a start symbol**. Non-terminals expand into other symbols, while terminals form the final valid strings. Regular grammars are efficient for pattern recognition and text processing because they can be directly converted into finite automata.

### **2. Finite Automaton (FA)**

A **finite automaton** is a computational model that recognizes patterns in input strings. It consists of states, an input alphabet, transitions between states, an initial state, and final states that determine acceptance. The automaton reads an input string one symbol at a time, transitioning between states based on predefined rules. **Deterministic finite automata** have exactly one transition per input symbol per state, ensuring a single computation path, whereas **non-deterministic finite automata** allow multiple possible transitions. Despite their differences, both types can recognize the same languages.

A string is recognized by a finite automaton if it starts in the initial state, follows valid transitions, and ends in an accepting state. This makes finite automata useful for lexical analysis, text search, and validation of structured input.




## Objectives:

1. Discover what a language is and what it needs to have in order to be considered a formal one;

2. Provide the initial setup for the evolving project that you will work on during this semester. You can deal with each laboratory work as a separate task or project to demonstrate your understanding of the given themes, but you also can deal with labs as stages of making your own big solution, your own project. Do the following:

    a. Create GitHub repository to deal with storing and updating your project;

    b. Choose a programming language. Pick one that will be easiest for dealing with your tasks, you need to learn how to solve the problem itself, not everything around the problem (like setting up the project, launching it correctly and etc.);

    c. Store reports separately in a way to make verification of your work simpler (duh)

3. According to your variant number, get the grammar definition and do the following:

    a. Implement a type/class for your grammar;

    b. Add one function that would generate 5 valid strings from the language expressed by your given grammar;

    c. Implement some functionality that would convert and object of type Grammar to one of type Finite Automaton;

    d. For the Finite Automaton, please add a method that checks if an input string can be obtained via the state transition from it;

## Implementation description

The Grammar class constructor defines the production rules that will be used to generate the words.

```
Grammar() {
    P['S'] = {"aB"};
    P['B'] = {"aC", "bB"};
    P['C'] = {"bB", "c", "aS"};
}
```

The generateString() function generates a random string based on a predefined grammar. It initializes a string containing only the start symbol and sets up a random number generator to select production rules. The function iterates over the string, looking for non-terminal symbols that need replacement. When a non-terminal is found, a random rule from P[c] is selected and used to replace the symbol. This process continues until all non-terminals are replaced with terminal symbols. The final string is stored and returned as a valid output. 

```
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
```
The same way th Grammar constructor defines the production rules the FiniteAutomaton class constructor defines the transition rules from one state to another
```
FiniteAutomaton() {
    transitions[{'S', 'a'}] = 'B';  // S → aB
    transitions[{'B', 'a'}] = 'C';  // B → aC
    transitions[{'B', 'b'}] = 'B';  // B → bB
    transitions[{'C', 'b'}] = 'B';  // C → bB
    transitions[{'C', 'c'}] = 'C';  // C → c (Final state)
    transitions[{'C', 'a'}] = 'S';  // C → aS
}
```
The validateString() function checks whether a given input string follows valid state transitions defined by a finite automaton. It starts with the initial state and processes the input symbol by symbol. If a valid transition exists for the current state and input symbol, the function moves to the next state. If no valid transition is found, it immediately returns false, indicating an invalid string. After processing all input symbols, the function checks whether the final state reached is an accepting state. If the final state is in the set of valid end states, the function returns true; otherwise, it returns false.
```
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
```


## Conclusions / Screenshots / Results

Understanding formal grammars and finite automata is essential for text processing, compilers, and structured input validation. Grammars define how strings are generated, while finite automata determine if a given input follows the correct pattern. The integration of these concepts ensures that structured data can be efficiently processed and validated.

The results from the implementation demonstrate that the generateString() function successfully produces structured strings following grammar rules, while the validateString() function correctly determines their validity based on finite automaton transitions. 
```
Generated Strings from Grammar:
Generated String 1: aac
Generated String 2: ababbbac
Generated String 3: aababbac
Generated String 4: aac
Generated String 5: abbbaaababbbaaabaaaac
```
```
Testing Strings in Finite Automaton:
String 'aac' is VALID.
String 'ababbbac' is VALID.
String 'aababbac' is VALID.
String 'aac' is VALID.
String 'abbbaaababbbaaabaaaac' is VALID.
```
## References
[Regular language.Finite automata](https://drive.google.com/file/d/1rBGyzDN5eWMXTNeUxLxmKsf7tyhHt9Jk/view?pli=1)