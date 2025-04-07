# Topic: Chomsky Normal Form
## **Student : Pleșca Denis**
## **Teacher : Crețu Dumitru**

---


## Introduction to Context-Free Grammars (CFGs)

A **Context-Free Grammar (CFG)** is a formal system used to describe the syntax of languages, particularly programming languages and natural languages. A CFG is composed of:

- **Terminals (T)**: The basic symbols of the language (e.g., `a`, `b`, `1`, `+`).
- **Non-terminals (N)**: Abstract symbols used to define grammatical structure (e.g., `S`, `A`, `B`).
- **Start Symbol (S)**: The initial non-terminal from which derivation starts.
- **Production Rules (P)**: Rules that define how non-terminals can be replaced by combinations of terminals and other non-terminals.

A CFG is formally represented as a 4-tuple \( G = (N, T, P, S) \).

CFGs play a central role in compilers, interpreters, and parsers. However, to simplify parsing and enable certain algorithmic processes like the CYK (Cocke–Younger–Kasami) parsing algorithm, it is helpful to convert a CFG to **Chomsky Normal Form (CNF)**.

---

## What is Chomsky Normal Form?

A grammar is said to be in **Chomsky Normal Form** if all its production rules are of the following forms:

1. \( A \rightarrow BC \)  — where \( A, B, C \in N \) (i.e., non-terminals only)
2. \( A \rightarrow a \)   — where \( a \in T \) (i.e., a single terminal)
3. \( S \rightarrow \varepsilon \) — (only if \( \varepsilon \) is part of the language, and only from the start symbol \( S \))

> **Note:** \( \varepsilon \) denotes the empty string.

Any CFG can be converted into an equivalent CNF grammar that generates the same language (excluding or including the empty string).

---

## Why Use CNF?

Converting a grammar to CNF simplifies algorithms such as:

- **CYK Algorithm**: Efficient parsing of strings using dynamic programming.
- **Proving properties**: Useful in proofs for decidability and language recognition.
- **Automation**: Easier for tools and software to handle.

CNF also enforces consistency and minimalism in production rules, which helps in building efficient parsers and syntax analyzers.

---

## Steps to Convert a Grammar to CNF

The general steps for transforming a CFG into CNF are:

### 1. Remove Null (\( \varepsilon \)) Productions

- Identify all nullable non-terminals (those that can derive \( \varepsilon \)).
- For each rule containing nullable non-terminals, generate new rules by omitting nullable symbols in all possible combinations.
- Remove original \( \varepsilon \) rules except for the start symbol.

### 2. Remove Unit Productions

- Rules of the form \( A \rightarrow B \) (where both are non-terminals) are replaced by copying all rules from \( B \) to \( A \).
- Transitive unit chains are collapsed to avoid recursion.

### 3. Remove Useless Symbols

- Eliminate symbols that do not contribute to a terminal string (non-generating symbols).
- Remove unreachable symbols (not accessible from the start symbol).

### 4. Convert Terminal Symbols in Long Productions

- Any rule like \( A \rightarrow aB \) is broken into two:
  - Introduce a new non-terminal: \( X \rightarrow a \)
  - Replace the terminal with its non-terminal: \( A \rightarrow XB \)

### 5. Break Down Long Productions

- Convert rules with more than 2 symbols on the right-hand side into binary productions by introducing intermediate variables.
- Example: \( A \rightarrow BCD \) becomes:
  - \( A \rightarrow BX_1 \)
  - \( X_1 \rightarrow CD \)

---

## Code Implementation Highlights

The provided implementation encapsulates the CNF conversion in a `CNFConverter` class, which performs all 5 steps described above:

- `removeNullProductions()` — Detects and removes nullable productions.
- `removeUnitProductions()` — Replaces unit productions with direct rules.
- `removeUselessSymbols()` — Discards unreachable or non-generating rules.
- `convertTerminalsInRules()` — Ensures that only terminals appear in terminal-only rules.
- `convertLongProductions()` — Breaks long rules into binary rules.

Each transformation maintains the language equivalence, ensuring the resulting grammar generates the same set of strings.

---

## Example Grammar

Given this input CFG:

```text
S → b A C | B
A → a | a S | b C a C b
B → A C | b S | a A a
C → ~ | A B
E → B A
```

The code systematically transforms it into CNF form by:

1. Replacing \( ~ \) (null/epsilon) with combinations without nullable symbols
2. Collapsing rules like \( A \rightarrow B \) into direct rules
3. Eliminating useless symbols like `E` if unreachable
4. Replacing terminals in long rules with intermediate variables (e.g., \( a \rightarrow X1 \))
5. Breaking sequences into binary productions

---

## Output Format of CNF

The CNF grammar is printed in the form:

```text
NonTerminal -> Symbol Symbol
NonTerminal -> Terminal
```

Each rule satisfies CNF requirements:

- Either exactly **two non-terminals** on the right
- Or a **single terminal**

---

## Benefits of Generalized CNF Conversion

This CNF converter supports **any CFG**, not just a specific variant. This gives the implementation a practical advantage for:

- Teaching tools and academic analysis
- Grammar preprocessors in compiler design
- Building foundations for syntax analyzers
- Automating formal language transformations


---

## Code Snippet Explanations

### Grammar Type Definitions
```cpp
using Symbol = std::string;
using Production = std::vector<Symbol>;
using Grammar = std::unordered_map<Symbol, std::vector<Production>>;
```
Defines convenient aliases for grammar structures:
- `Symbol` represents either a terminal or non-terminal.
- `Production` is a sequence of symbols.
- `Grammar` maps non-terminals to their possible productions.

---

### Constructor
```cpp
CNFConverter(const Grammar& input, const Symbol& start)
    : grammar(input), startSymbol(start), newVarCount(0) {}
```
Initializes the converter with:
- `grammar`: input CFG.
- `startSymbol`: entry point for derivations.
- `newVarCount`: counter to generate new unique variable names.

---

### Generating Unique Variables
```cpp
Symbol getNewVariable() {
    return "X" + std::to_string(++newVarCount);
}
```
Used to create new non-terminals like `X1`, `X2`, etc., when breaking long rules or replacing terminals.

---

### Removing Null Productions
```cpp
void removeNullProductions() { ... }
```
- Detects nullable symbols (producing `~`).
- Generates all combinations of rules excluding nullable symbols.
- Eliminates explicit `~` rules (unless required).

---

### Removing Unit Productions
```cpp
void removeUnitProductions() { ... }
```
- Replaces rules like `A → B` by copying all of `B`’s productions into `A`.
- Handles transitive chains (e.g., `A → B → C`).

---

### Removing Useless Symbols
```cpp
void removeUselessSymbols() { ... }
```
- Removes non-generating and unreachable symbols.
- Ensures grammar only includes symbols contributing to terminal strings from the start symbol.

---

### Replacing Terminals in Long Productions
```cpp
void convertTerminalsInRules() { ... }
```
- Introduces new variables for terminals in long productions.
- Ensures that only terminals are alone on the right-hand side.

Example:
```cpp
b → X1
X1 → b
```

---

### Converting Long Rules to Binary
```cpp
void convertLongProductions() { ... }
```
- Breaks rules like `A → B C D` into binary forms:
```cpp
A → B X1
X1 → C D
```
- Uses helper map to reuse binary nodes.

---

### Checking Symbol Type
```cpp
bool isNonTerminal(const Symbol& s) {
    return !s.empty() && std::isupper(s[0]);
}
```
Determines if a symbol is a non-terminal (starts with uppercase letter).


---

## Conclusion

Chomsky Normal Form is a standardized and minimal representation of context-free grammars that simplifies computation and parsing. The CNF transformation process involves systematically removing complexity and enforcing binary productions.

By implementing each conversion step in a clean, modular, and reusable class, the provided code demonstrates not just the theoretical correctness but also practical utility. This converter is fully generalized, extensible, and capable of handling any input CFG for transformation into CNF.

Such a tool is instrumental in advancing understanding of automata theory and formal languages and is a key component in the construction of compilers and interpreters.