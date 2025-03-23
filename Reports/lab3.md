### **Lexer & Scanner**
### **Student : Pleșca Denis**
### **Teacher : Crețu Dumitru**

## **1. Introduction**
Lexical analysis, often referred to as scanning or tokenization, is a fundamental phase in the process of compiling or interpreting programming languages. During this phase, a sequence of raw characters from the source code is broken down into manageable units called tokens. These tokens provide meaningful information for later stages of interpretation, such as parsing and code generation.

The goal of this report is to document the creation of a lexer for a domain-specific language (DSL)  designed to process time-series data through operations like forecasting, streaming, plotting, and cleaning. Unlike many basic lexers used in student projects (such as calculator expression tokenizers), the lexer handles a rich set of keywords, operators, and custom syntax constructs. This lexer was implemented in C++ for greater performance and learning value, avoiding external libraries like ANTLR or Flex.

By building the lexer from scratch, this project aims to deepen understanding of how real-world interpreters function under the hood, particularly the transformation of a raw character stream into structured, typed tokens with positional metadata.

## **2. Theory**

### **2.1 Lexemes vs. Tokens**
A **lexeme** is the smallest sequence of characters in the input that forms a valid unit, such as a word or symbol. A **token**, on the other hand, is a classified version of that lexeme—tagged with a category like `ID`, `INT`, or `STRING`, along with optional metadata like its position in the source code.

For example, given the line:
```
LOAD salesData FROM "sales.csv"
```
We have the following lexemes: `LOAD`, `salesData`, `FROM`, `"sales.csv"`.
These lexemes are transformed into tokens: `LOAD`, `ID(salesData)`, `FROM`, `STRING("sales.csv")`.

### **2.2 Role of the Lexer**
A lexer (or tokenizer) is the first step in interpreting or compiling code. Its job is to:
- Read characters from the input
- Skip irrelevant whitespace
- Recognize patterns (identifiers, keywords, strings, numbers)
- Convert them into structured tokens with type and location
- Emit an ordered stream of tokens for the parser to consume

This process is essential for validating syntax and enabling subsequent parsing.

## **3. Key Code Snippets and Explanations**

### **3.1 Token Struct**
```cpp
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
};
```
This snippet defines the basic building block of the lexer's output: the token. It encapsulates the type (such as `ID`, `STRING`, or `INT`), the original value from the source code, and the line/column position for error tracking. The constructor simplifies the creation of new tokens.

---

### **3.2 Lexer Constructor and REPL**
```cpp
Lexer::Lexer(const std::string& input) : input(input) {}

void Lexer::runREPL() {
    std::string input, line;
    while (std::getline(std::cin, line)) input += line + '\n';
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        std::cout << tokenTypeToString(token.type) << "('" << token.value << "') at line "
                  << token.line << ", col " << token.column << "\n";
    }
}
```
The constructor stores the input source as a string. The REPL (Read-Eval-Print Loop) enables real-time input and outputs each token with its type and position. This provides instant feedback, aiding both debugging and demonstration.

---

### **3.3 Character Handling Helpers**
```cpp
char Lexer::peek() const {
    return pos < input.size() ? input[pos] : '\0';
}

char Lexer::peekNext() const {
    return pos + 1 < input.size() ? input[pos + 1] : '\0';
}

char Lexer::advance() {
    column++;
    return input[pos++];
}
```
These utility functions are essential for controlled traversal of the input string. `peek()` returns the current character without consuming it, `peekNext()` looks ahead, and `advance()` consumes a character while updating the column counter.

---

### **3.4 Whitespace Skipping**
```cpp
void Lexer::skipWhitespace() {
    while (pos < input.size() && std::isspace(input[pos])) {
        if (input[pos] == '\n') { line++; column = 1; }
        else { column++; }
        pos++;
    }
}
```
Whitespace characters such as spaces, tabs, and newlines are not meaningful in most ChronoLang statements. This function skips over them but also tracks line and column changes to maintain accurate position reporting for each token.

---

### **3.5 Token Generation Entry Point**
```cpp
Token Lexer::nextToken() {
    skipWhitespace();
    if (pos >= input.size()) return makeToken(TokenType::END_OF_FILE, "");

    char c = peek();
    if (std::isalpha(c) || c == '_') return makeIdentifierOrKeyword();
    if (std::isdigit(c)) return makeNumber();
    if (c == '"') return makeString();
    return makeSymbol();
}
```
This method serves as the central logic for token generation. After removing whitespace, it inspects the current character and delegates to the appropriate method for keywords, numbers, strings, or symbols. It also ensures that when input ends, an EOF token is returned.

---

### **3.6 Token Factory Method**
```cpp
Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, line, column);
}
```
Creates and returns a `Token` object using the current lexer's context. It ensures that all generated tokens carry accurate positional metadata, which is useful for error messages in later compiler stages.

---

### **3.7 Tokenizing Time Units**
```cpp
if (value == "d" || value == "h" || value == "m") {
    return makeToken(TokenType::TIME_UNIT, value);
}
```
ChronoLang includes domain-specific units for time such as days (`d`), hours (`h`), and minutes (`m`). This code snippet identifies and classifies them accordingly when they appear in expressions like `10d` or `3h`.

---

### **3.8 End-of-File Token**
```cpp
if (pos >= input.size()) {
    return makeToken(TokenType::END_OF_FILE, "");
}
```
When the lexer reaches the end of the input string, it emits an `END_OF_FILE` token. This is crucial for parsers to know when to stop reading and prevents out-of-bounds access.

---

### **3.9 Invalid Symbol Handling**
```cpp
return makeToken(TokenType::INVALID, std::string(1, c));
```
If the lexer encounters an unknown character that doesn't match any rule, it classifies it as an `INVALID` token. This allows the system to provide meaningful error feedback instead of crashing or silently skipping errors.

---

### **3.10 Sample Input and Token Stream**
**Input:**
```text
LOAD data FROM "data.csv"
SET WINDOW = 10d
```

**Expected Token Stream:**
```
LOAD('LOAD') at line 1, col 1
ID('data') at line 1, col 6
FROM('FROM') at line 1, col 11
STRING('data.csv') at line 1, col 16
SET('SET') at line 2, col 1
WINDOW('WINDOW') at line 2, col 5
=('=') at line 2, col 12
INT('10') at line 2, col 14
TIME_UNIT('d') at line 2, col 16
EOF('') at line 2, col 17
```
This example illustrates the real output of the lexer. It breaks down the DSL input into meaningful tokens while recording their exact location. This provides precise control over syntax validation.

---

### **3.11 Handling Arrow Operator (`->`)**
```cpp
case '-':
    if (match('>')) return makeToken(TokenType::ARROW, "->");
    break;
```
The `->` operator is used in ChronoLang for linking transformations, like `TREND(column) -> forecast_next(5d)`. This logic ensures that both characters are matched together to form one `ARROW` token.

---

### **3.12 Matching Delimiters and Operators**
```cpp
case '=': return makeToken(TokenType::EQUAL, "=");
case '{': return makeToken(TokenType::LBRACE, "{");
case '}': return makeToken(TokenType::RBRACE, "}");
case '(': return makeToken(TokenType::LPAREN, "(");
case ')': return makeToken(TokenType::RPAREN, ")");
case ',': return makeToken(TokenType::COMMA, ",");
```
These cases recognize and return tokens for syntax symbols frequently used in ChronoLang. Each symbol gets mapped to its corresponding `TokenType`, ensuring the language syntax is correctly parsed later.

---

## **4. Conclusion**
By building the lexer from scratch in C++, the implementation showcases precise control over tokenization while reinforcing core concepts like lexeme recognition, keyword matching, and handling of string literals and numbers.

The lexer effectively tokenizes DSL input supporting commands such as `LOAD`, `SET`, `FORECAST`, `PLOT`, and control flow constructs like loops. The REPL-driven approach allows for live feedback during development and provides a strong foundation for further stages like syntax parsing and AST generation.

Future work could involve extending the lexer to handle:
- Escaped characters in strings
- Inline or block comments
- UTF-8 support and advanced error diagnostics


