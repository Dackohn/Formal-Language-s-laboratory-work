# Topic: Regular expressions
## **Student : Pleșca Denis**
## **Teacher : Crețu Dumitru**

---

## What Are Regular Expressions?

**Regular expressions**, often abbreviated as **regex** or **regexp**, are sequences of characters that define search patterns. Originally developed for formal language theory and automata, regex has become a powerful tool in text processing, pattern recognition, and string manipulation. 

They allow for compact expression of complex string-matching rules and are widely supported in programming languages, scripting tools, and data-processing libraries.

At their core, regular expressions are used to **search**, **match**, and **manipulate** text according to defined syntactic patterns.

---

## Formal Foundations

The theoretical basis for regular expressions comes from **automata theory** and **formal language theory**. In this context:

- A **regular language** is a language that can be expressed using a **finite automaton**.
- Regular expressions provide a declarative way to define such languages.

There are three core operations in formal regular expressions:

1. **Concatenation** – `AB` matches an A followed by a B.
2. **Alternation (Union)** – `A|B` matches either A or B.
3. **Kleene Star** – `A*` matches zero or more repetitions of A.

These simple operations can express a wide range of useful patterns, and they correspond to constructions in finite automata.

---

## Common Regex Syntax in Practice

| Symbol | Meaning                           | Example     |
|--------|-----------------------------------|-------------|
| `.`    | Any character except newline       | `a.b` → `acb`, `axb` |
| `*`    | 0 or more repetitions              | `a*` → `""`, `"a"`, `"aaaa"` |
| `+`    | 1 or more repetitions              | `a+` → `"a"`, `"aaa"` |
| `?`    | 0 or 1 occurrence                  | `a?` → `""`, `"a"` |
| `|`    | Alternation                        | `a|b` → `"a"` or `"b"` |
| `()`   | Grouping                          | `(ab)+` → `"ab"`, `"abab"` |
| `[]`   | Character class                    | `[a-c]` → `"a"`, `"b"`, `"c"` |

> Note: In this project, we use custom notation like `^+`, `^*`, and explicit numerical powers to limit repetitions and control expression size.

---

## Applications of Regular Expressions

Regular expressions are ubiquitous in computer science, used in:

- **Validation**: Email, phone number, credit card number formats.
- **Lexical analysis**: Tokenizing programming languages and compilers.
- **Data cleaning**: Removing unwanted characters, whitespace, or malformed records.
- **Information retrieval**: Search tools like `grep`, `sed`, and advanced search engines.
- **Security auditing**: Pattern matching in logs to detect anomalies or intrusions.
- **Natural language processing (NLP)**: Rule-based token recognition.

---

## From Matching to Generation

While regex is traditionally used for *matching* or *extracting* patterns from text, this project tackles the **reverse problem**: *generating valid strings that conform to a given regular expression*.

This reverse approach has valuable uses in:

- **Fuzz testing**: Automatically generating test inputs for programs.
- **Simulation**: Modeling random behavior based on rules.
- **Grammar-based generation**: Scripting dialogue, data sequences, or test cases.

---

## Abstract Syntax Tree (AST) Interpretation

To dynamically generate valid strings from a regex, we first **parse** the expression into a tree-like structure known as an **Abstract Syntax Tree (AST)**.

Each node in the tree represents a fundamental regex operation:

- **Literal node** – Represents a single character or symbol.
- **Sequence node** – Represents concatenated elements.
- **Alternation node** – Represents a choice (i.e., `A|B`).
- **Repetition node** – Represents repetition with bounds (`*`, `+`, `?`, or fixed amounts).

Using recursive descent parsing, the regex is converted into this tree. Then, during generation, we **traverse the tree**, applying randomization where needed to respect alternations and repetition ranges.

---

## Managing Repetition and Expression Growth

A key concern in regex generation is **infinite repetition**. For instance:

- `A*` theoretically allows for infinitely long strings.
- `A+` enforces at least one repetition, but can still be unbounded.

To avoid runaway generation, this implementation imposes a hard **limit of 5** on all open-ended repetitions like `*` and `+`. This ensures that generated strings are:

- Finite
- Realistically sized
- Easy to interpret and analyze

---

## Custom Notation Used in This Project

This implementation introduces a slightly modified syntax to simplify parsing and enhance control:

| Notation   | Meaning                                  |
|------------|------------------------------------------|
| `^+`       | Repeat 1 to 5 times                      |
| `^*`       | Repeat 0 to 5 times                      |
| `^n`       | Repeat exactly `n` times                 |
| `?`        | Optional (0 or 1 time)                   |
| `(A|B|C)`  | Alternation inside group                 |
| `()`       | Grouping used for complex expressions    |

---

## Why Use AST-Based Generation?

There are several benefits of using ASTs for regex generation:

1. **Modularity**: Easily represent and isolate components of a regex.
2. **Recursion**: Perfect fit for recursive grammar structures.
3. **Random generation**: Allows for controlled random choice during alternation or repetition.
4. **Extensibility**: The parser can be extended to support more operators or more complex grammar rules in the future.

---

## Limitations and Considerations

While the AST-based approach is effective, there are trade-offs:

- Not all regex syntax is supported (e.g., character classes like `[a-z]`, lookaheads, backreferences).
- Infinite languages must be artificially bounded.
- Expressions must be syntactically well-formed; otherwise, parsing errors may occur.

These limitations are acceptable for learning purposes and for generating predictable outputs.

---
# Code Walkthrough: Regex Generator

This section provides annotated code snippets to help understand the inner workings of the C++ program that **parses** a custom regular expression and **generates valid strings**.

---

## Constants and Random Generator Setup

```cpp
const int MAX_REPETITION = 5;
std::mt19937 rng(time(nullptr));
```

- `MAX_REPETITION`: Limits how many times something can repeat (used for `+`, `*`, etc.)
- `rng`: Mersenne Twister random generator seeded by the current time.

```cpp
int rand_int(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}
```

- Random number utility used for choosing how many times to repeat or which branch to pick.

---

## AST Node Structure

```cpp
struct Node {
    enum Type { LITERAL, SEQUENCE, ALTERNATION, REPETITION } type;
    std::string value;
    std::vector<std::shared_ptr<Node>> children;
    int repeat_min = 1, repeat_max = 1;
};
```

- This structure is the backbone of the regex parser.
  - `LITERAL`: a single character like `A` or `B`
  - `SEQUENCE`: a sequence of sub-patterns like `ABC`
  - `ALTERNATION`: like `(A|B|C)`
  - `REPETITION`: like `A^+`, `A^3`, etc.
- Repetitions are bounded using `repeat_min` and `repeat_max`.

---

## Parsing Logic

### Basic Setup

```cpp
class Parser {
    std::string regex;
    size_t pos = 0;

public:
    Parser(std::string input) : regex(std::move(input)) {}
    std::shared_ptr<Node> parse() {
        return parse_sequence();
    }
```

- The parser walks through a string representation of a regex and builds a tree.

### Character Lookahead

```cpp
char peek() const { return pos < regex.size() ? regex[pos] : '\0'; }
char get() { return pos < regex.size() ? regex[pos++] : '\0'; }
```

- `peek()` lets you look at the next character without consuming it.
- `get()` actually reads it and moves forward.

---

## Parsing Elements

```cpp
std::shared_ptr<Node> parse_sequence(char until = '\0');
std::shared_ptr<Node> parse_alternation();
std::shared_ptr<Node> parse_literal();
void apply_power(std::shared_ptr<Node> &node);
```

- `parse_sequence()` reads symbols one by one and joins them.
- `parse_alternation()` handles `A|B|C`-type cases.
- `parse_literal()` creates nodes for individual characters.
- `apply_power()` handles `^+`, `^*`, `^3`, `?` and wraps the node in a repetition node.

---

## Repetition Logic

```cpp
void apply_power(std::shared_ptr<Node> &node) {
    if (peek() == '^') {
        get();
        if (peek() == '+') {
            get();
            node->repeat_min = 1;
            node->repeat_max = MAX_REPETITION;
        } else if (peek() == '*') {
            get();
            node->repeat_min = 0;
            node->repeat_max = MAX_REPETITION;
        } else {
            std::string num;
            while (isdigit(peek())) num += get();
            int n = std::stoi(num);
            node->repeat_min = node->repeat_max = n;
        }
        auto rep_node = std::make_shared<Node>(Node::REPETITION);
        rep_node->repeat_min = node->repeat_min;
        rep_node->repeat_max = node->repeat_max;
        rep_node->children.push_back(node);
        node = rep_node;
    } else if (peek() == '?') {
        get();
        auto rep_node = std::make_shared<Node>(Node::REPETITION);
        rep_node->repeat_min = 0;
        rep_node->repeat_max = 1;
        rep_node->children.push_back(node);
        node = rep_node;
    }
}
```

- Applies repetition modifiers to any node.
- Handles `^+`, `^*`, `^3`, and `?` using a switch-like structure.

---

## String Generation Logic

```cpp
std::string generate(const std::shared_ptr<Node> &node) {
    switch (node->type) {
        case Node::LITERAL:
            return node->value;
        case Node::SEQUENCE: {
            std::string result;
            for (const auto &child : node->children)
                result += generate(child);
            return result;
        }
        case Node::ALTERNATION: {
            int choice = rand_int(0, node->children.size() - 1);
            return generate(node->children[choice]);
        }
        case Node::REPETITION: {
            int times = rand_int(node->repeat_min, node->repeat_max);
            std::string result;
            for (int i = 0; i < times; ++i)
                result += generate(node->children[0]);
            return result;
        }
    }
    return "";
}
```

- This function is recursive and constructs a valid string by walking the AST.
- Each node type behaves differently:
  - Sequence → append children
  - Alternation → randomly choose one
  - Repetition → randomly choose how many times to repeat

---

## Main Function

```cpp
int main() {
    std::vector<std::string> regexes = {
        "O(P|Q2|R)^+2(3|4)",
        "A^*B(C|D|E)F(G|H|I)^2",
        "J^+K(L|M|N)^*0?(P|Q)^3"
    };

    for (const auto &r : regexes) {
        try {
            Parser parser(r);
            auto ast = parser.parse();
            std::string word = generate(ast);
            std::cout << "Regex: " << r << " => " << word << "\n";
        } catch (const std::exception &e) {
            std::cerr << "Error parsing regex '" << r << "': " << e.what() << "\n";
        }
    }
    return 0;
}
```

- Three sample expressions are parsed and used to generate strings.
- Each one prints:
  `Regex: A^*B(C|D|E)... => validWord`

---

## Summary

This parser-generator pipeline transforms a regex into an AST, then walks the tree to generate a random valid string. It's a great introduction to:

- Parsing techniques
- AST manipulation
- Recursive evaluation
- Simplified regex semantics for controlled generation


---

## Conclusion

Regular expressions are powerful tools with deep theoretical and practical foundations. Reversing their usage from pattern-matching to **string generation** opens up new creative and technical opportunities.

By converting regular expressions into ASTs, we gain a flexible, extensible structure that allows for controlled, random generation of valid strings. This method has implications in testing, simulation, education, and automated data generation — bridging the gap between formal theory and real-world utility.
