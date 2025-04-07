# Topic: Chomsky Normal Form
## **Student : Pleșca Denis**
## **Teacher : Crețu Dumitru**


## Introduction

Lexical and syntactic analysis are fundamental components of programming language processing. They are the first stages in the transformation of high-level source code or formal expressions into structures that can be interpreted, executed, or compiled. These processes are crucial not only in compilers but also in interpreters, code analyzers, and domain-specific language (DSL) processors. In this report, we explore the theoretical underpinnings of these two stages with a focus on their application to regular expression parsing using C++.

## What is Parsing?

Parsing is the process of analyzing a sequence of symbols (usually tokens) to determine its grammatical structure with respect to a given formal grammar. The primary goal of parsing is to determine if the input string conforms to the grammar and to construct a data structure (typically a parse tree or abstract syntax tree) that represents the syntactic structure of the input.

### Parsing in Practice

Parsing involves two primary components:
- **Lexical Analysis (Lexing)**: This breaks the input into tokens (basic syntactic units such as identifiers, literals, operators, etc.).
- **Syntactic Analysis (Parsing)**: This builds a tree structure that reflects the grammatical structure of the token stream.

In compiler design, this process is usually divided among a **lexer** (tokenizer) and a **parser**, each responsible for different levels of abstraction.

### Formal Grammar
A grammar consists of:
- **Terminals**: Basic symbols from which strings are formed (e.g., characters, tokens).
- **Non-terminals**: Variables that denote sets of strings.
- **Production Rules**: Rules for replacing non-terminals with combinations of terminals and non-terminals.
- **Start Symbol**: The entry point of the grammar.

A context-free grammar (CFG) is often used in parser implementations due to its expressive power and efficient parsing algorithms.

## Abstract Syntax Tree (AST)

An Abstract Syntax Tree (AST) is a tree representation of the abstract syntactic structure of source code. Unlike parse trees, ASTs do not include every detail of the grammar, only the information essential for later stages such as semantic analysis or code generation.

### Why Use an AST?
- **Simplification**: Removes extraneous grammar rules (e.g., parentheses, associative operators).
- **Efficiency**: Easier to manipulate than full parse trees.
- **Foundation**: Serves as the basis for optimization and transformation phases.

### AST Structure
Each node in an AST represents a construct occurring in the source. For example, in regular expression parsing:
- A literal (e.g., 'A') is a leaf node.
- A sequence (e.g., "AB") is a parent node with children.
- An alternation (e.g., "A|B") has a node with two or more options.
- A repetition (e.g., "A^+", "A^3") indicates that a sub-node should be repeated.

## Lexical Analysis Using Regular Expressions

Regular expressions (regex) are patterns used to match character combinations in strings. They are powerful tools for implementing lexical analyzers, as many programming languages and libraries support them natively.

### Regular Expression Constructs
- `[...]`: Character class
- `*`: Zero or more repetitions
- `+`: One or more repetitions
- `?`: Zero or one (optional)
- `|`: Alternation (logical OR)
- `(...)`: Grouping

### Tokenization with Regex
In the context of regular expression parsing, token types can include:
- **Literal**: A single alphanumeric character
- **Operators**: `|`, `^`, `?`
- **Grouping**: `(` and `)`
- **Quantifiers**: Numeric values representing repetitions

Regular expressions are matched sequentially to extract and classify tokens. The order of matching is crucial, as some patterns (like digits) may overlap with others (like literals).

## Syntactic Analysis with Recursive Descent Parsing

Recursive descent parsing is a top-down parsing technique that implements a grammar using mutually recursive procedures. Each non-terminal in the grammar corresponds to a function in the parser.

### Key Concepts
- **Backtracking**: Trying multiple possibilities when rules have ambiguity.
- **Lookahead**: Inspecting the next token(s) to decide which rule to apply.
- **Operator Precedence**: Properly handling sequences, alternations, and repetitions.

### Grammar for Regex Parser
An example grammar used in the parser:
```
expression ::= sequence
sequence   ::= (group | literal)*
group      ::= '(' alternation ')'
alternation::= sequence ('|' sequence)*
literal    ::= [A-Z0-9]
```
This grammar supports grouping, alternation, and basic repetition.

### Constructing the AST

As the parser reads tokens, it constructs nodes and attaches them according to the grammar rules. Operator precedence is maintained by the recursive structure: for example, grouping happens before alternation, and alternation happens before sequencing.

---
## Code Snippets and Explanations

### Lexical Analysis and Token Handling

#### TokenType Enum
The `TokenType` enum defines the possible categories of tokens that the lexer can recognize in the input regular expression. These categories are used during parsing to apply grammar rules.

```cpp
enum class TokenType {
    LITERAL,
    LPAREN,
    RPAREN,
    ALTERNATION,  // |
    STAR,         // ^
    QMARK,        // ?
    NUMBER,
    END
};
```

#### Token Structure
Each token is represented as a structure holding a token type and its textual value. This allows both classification and access to the original content.

```cpp
struct Token {
    TokenType type;
    std::string value;
};
```

#### Lexer Using Regular Expressions
The lexer reads through the input string and uses a single combined regular expression pattern to identify all valid tokens. It captures literals, parentheses, operators, and numeric quantifiers. It also ensures that each match occurs at the beginning of the remaining string to avoid skipping invalid characters.

```cpp
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::regex token_patterns("([A-Z0-9])|([()])|(\|)|(\^)|(\?)|([0-9]+)");
    std::smatch match;
    std::string s = input;

    while (!s.empty()) {
        if (std::regex_search(s, match, token_patterns)) {
            if (match.position() != 0) {
                throw std::runtime_error("Unexpected character: " + s);
            }

            if (match[1].matched) tokens.push_back({TokenType::LITERAL, match[1]});
            else if (match[2] == "(") tokens.push_back({TokenType::LPAREN, "("});
            else if (match[2] == ")") tokens.push_back({TokenType::RPAREN, ")"});
            else if (match[3].matched) tokens.push_back({TokenType::ALTERNATION, "|"});
            else if (match[4].matched) tokens.push_back({TokenType::STAR, "^"});
            else if (match[5].matched) tokens.push_back({TokenType::QMARK, "?"});
            else if (match[6].matched) tokens.push_back({TokenType::NUMBER, match[6]});

            s = match.suffix();
        } else {
            throw std::runtime_error("Invalid token: " + s);
        }
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}
```

#### AST Node Structure
Each node in the Abstract Syntax Tree (AST) represents a component of the parsed regular expression. The node type indicates whether it is a literal character, a sequence of expressions, an alternation (e.g., `A|B`), or a repetition (e.g., `A^+`).

```cpp
struct Node {
    enum Type { LITERAL, SEQUENCE, ALTERNATION, REPETITION } type;
    std::string value;
    std::vector<std::shared_ptr<Node>> children;
    int repeat_min = 1, repeat_max = 1;

    Node(Type t) : type(t) {}
};
```

The structure allows easy representation and recursive traversal of complex regular expressions in their syntactic form.

### Parsing and String Generation

#### Recursive Descent Parser
The parser processes the token stream and builds the Abstract Syntax Tree (AST) based on the grammar rules using a recursive descent method.

```cpp
class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    std::shared_ptr<Node> parse() {
        return parse_sequence();
    }

private:
    Token peek() const { return tokens[pos]; }
    Token get() { return tokens[pos++]; }
    bool match(TokenType type) {
        if (peek().type == type) {
            get();
            return true;
        }
        return false;
    }
```

#### Parse Literal and Group
Parses a single character or a grouped expression inside parentheses.

```cpp
    std::shared_ptr<Node> parse_literal() {
        Token t = get();
        auto node = std::make_shared<Node>(Node::LITERAL);
        node->value = t.value;
        return node;
    }

    std::shared_ptr<Node> parse_group() {
        match(TokenType::LPAREN);
        auto alt = parse_alternation();
        match(TokenType::RPAREN);
        return alt;
    }
```

#### Parse Sequence and Alternation
Handles sequences of elements and alternation (e.g., `A|B`).

```cpp
    std::shared_ptr<Node> parse_sequence() {
        auto node = std::make_shared<Node>(Node::SEQUENCE);
        while (peek().type != TokenType::RPAREN && peek().type != TokenType::END && peek().type != TokenType::ALTERNATION) {
            auto child = (peek().type == TokenType::LPAREN) ? parse_group() : parse_literal();
            apply_repetition(child);
            node->children.push_back(child);
        }
        if (node->children.size() == 1) return node->children[0];
        return node;
    }

    std::shared_ptr<Node> parse_alternation() {
        auto node = std::make_shared<Node>(Node::ALTERNATION);
        node->children.push_back(parse_sequence());
        while (match(TokenType::ALTERNATION)) {
            node->children.push_back(parse_sequence());
        }
        return node;
    }
```

#### Apply Repetition Modifiers
Handles repetition constructs like `^+`, `^3`, and `?` by wrapping nodes in repetition containers.

```cpp
    void apply_repetition(std::shared_ptr<Node>& node) {
        if (match(TokenType::STAR)) {
            if (peek().type == TokenType::NUMBER) {
                int n = std::stoi(get().value);
                node->repeat_min = node->repeat_max = n;
            } else {
                node->repeat_min = 1;
                node->repeat_max = MAX_REPETITION;
            }
            auto rep_node = std::make_shared<Node>(Node::REPETITION);
            rep_node->repeat_min = node->repeat_min;
            rep_node->repeat_max = node->repeat_max;
            rep_node->children.push_back(node);
            node = rep_node;
        } else if (match(TokenType::QMARK)) {
            auto rep_node = std::make_shared<Node>(Node::REPETITION);
            rep_node->repeat_min = 0;
            rep_node->repeat_max = 1;
            rep_node->children.push_back(node);
            node = rep_node;
        }
    }
};
```

#### String Generation from AST
The generator function walks the AST and produces a valid string based on its structure. It randomly selects from alternatives and determines repetition counts using the defined bounds.

```cpp
std::string generate(const std::shared_ptr<Node>& node) {
    switch (node->type) {
        case Node::LITERAL:
            return node->value;
        case Node::SEQUENCE: {
            std::string result;
            for (const auto& child : node->children)
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

This concludes the second category covering the parser and generation logic. These components work together to analyze and evaluate structured regular expressions.



---
## Summary
The implementation of a lexer and parser for regular expressions in C++ demonstrates core compiler construction techniques. The lexer uses regular expressions to identify token types, and the parser constructs an AST using recursive descent techniques. This provides a strong foundation for understanding how programming languages and formal systems can be parsed and interpreted efficiently.