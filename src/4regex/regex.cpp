#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <sstream>
#include <cctype>

const int MAX_REPETITION = 5;

std::mt19937 rng(time(nullptr));

int rand_int(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

std::ostringstream trace_log;

void trace(const std::string& msg) {
    trace_log << msg << "\n";
}

struct Node {
    enum Type { LITERAL, SEQUENCE, ALTERNATION, REPETITION } type;
    std::string value;
    std::vector<std::shared_ptr<Node>> children;
    int repeat_min = 1, repeat_max = 1;

    Node(Type t) : type(t) {}
};

class Parser {
    std::string regex;
    size_t pos = 0;

public:
    Parser(std::string input) : regex(std::move(input)) {}

    std::shared_ptr<Node> parse() {
        trace("Start parsing: " + regex);
        auto result = parse_sequence();
        trace("Finished parsing.");
        return result;
    }

private:
    char peek() const {
        return pos < regex.size() ? regex[pos] : '\0';
    }

    char get() {
        return pos < regex.size() ? regex[pos++] : '\0';
    }

    bool match(char expected) {
        if (peek() == expected) {
            get();
            return true;
        }
        return false;
    }

    std::shared_ptr<Node> parse_sequence(char until = '\0') {
        auto seq_node = std::make_shared<Node>(Node::SEQUENCE);
        trace("Begin sequence");
        while (pos < regex.size() && peek() != until && peek() != '|') {
            if (peek() == '(') {
                get();
                auto alt = parse_alternation();
                if (!match(')')) throw std::runtime_error("Expected )");
                apply_power(alt);
                seq_node->children.push_back(alt);
            } else {
                auto lit = parse_literal();
                apply_power(lit);
                seq_node->children.push_back(lit);
            }
        }
        trace("End sequence");
        if (seq_node->children.size() == 1) return seq_node->children[0];
        return seq_node;
    }

    std::shared_ptr<Node> parse_alternation() {
        trace("Begin alternation");
        auto alt_node = std::make_shared<Node>(Node::ALTERNATION);
        alt_node->children.push_back(parse_sequence(')'));
        while (match('|')) {
            alt_node->children.push_back(parse_sequence(')'));
        }
        trace("End alternation");
        return alt_node;
    }

    std::shared_ptr<Node> parse_literal() {
        char c = get();
        trace("Parsed literal: " + std::string(1, c));
        auto node = std::make_shared<Node>(Node::LITERAL);
        node->value = std::string(1, c);
        return node;
    }

    void apply_power(std::shared_ptr<Node>& node) {
        if (peek() == '^') {
            get();
            if (peek() == '+') {
                get();
                node->repeat_min = 1;
                node->repeat_max = MAX_REPETITION;
                trace("Applied ^+ to node");
            } else if (peek() == '*') {
                get();
                node->repeat_min = 0;
                node->repeat_max = MAX_REPETITION;
                trace("Applied ^* to node");
            } else {
                std::string num;
                while (isdigit(peek())) num += get();
                int n = std::stoi(num);
                node->repeat_min = node->repeat_max = n;
                trace("Applied ^" + std::to_string(n) + " to node");
            }
            auto rep_node = std::make_shared<Node>(Node::REPETITION);
            rep_node->repeat_min = node->repeat_min;
            rep_node->repeat_max = node->repeat_max;
            rep_node->children.push_back(node);
            node = rep_node;
        } else if (peek() == '?') {
            get();
            trace("Applied ? to node");
            auto rep_node = std::make_shared<Node>(Node::REPETITION);
            rep_node->repeat_min = 0;
            rep_node->repeat_max = 1;
            rep_node->children.push_back(node);
            node = rep_node;
        }
    }
};

std::string generate(const std::shared_ptr<Node>& node) {
    switch (node->type) {
        case Node::LITERAL:
            trace("Generating literal: " + node->value);
            return node->value;
        case Node::SEQUENCE: {
            trace("Generating sequence");
            std::string result;
            for (const auto& child : node->children)
                result += generate(child);
            return result;
        }
        case Node::ALTERNATION: {
            int choice = rand_int(0, node->children.size() - 1);
            trace("Generating alternation, chose option " + std::to_string(choice));
            return generate(node->children[choice]);
        }
        case Node::REPETITION: {
            int times = rand_int(node->repeat_min, node->repeat_max);
            trace("Generating repetition, times = " + std::to_string(times));
            std::string result;
            for (int i = 0; i < times; ++i)
                result += generate(node->children[0]);
            return result;
        }
    }
    return "";
}

int main() {
    std::vector<std::string> regexes = {
        "O(P|Q2|R)^+2(3|4)",
        "A^*B(C|D|E)F(G|H|I)^2",
        "J^+K(L|M|N)^*0?(P|Q)^3"
    };

    for (const auto& r : regexes) {
        try {
            Parser parser(r);
            auto ast = parser.parse();
            std::string word = generate(ast);
            std::cout << "Regex: " << r << " => " << word << "\n";
            std::cout << "Trace:\n" << trace_log.str() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error parsing regex '" << r << "': " << e.what() << "\n";
        }
        trace_log.str("");
        trace_log.clear();
    }

    return 0;
}
