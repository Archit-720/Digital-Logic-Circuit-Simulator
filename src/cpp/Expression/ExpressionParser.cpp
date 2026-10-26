#include "ExpressionParser.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <unordered_set>

using namespace std;

namespace {
string toUpper(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(toupper(c));
        });
    return text;
}

bool isIdentifierStart(char c) {
    return isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool isIdentifierPart(char c) {
    return isalnum(static_cast<unsigned char>(c)) || c == '_';
}

int precedence(const string& token) {
    if (token == "NOT") return 5;
    if (token == "AND" || token == "NAND") return 4;
    if (token == "XOR" || token == "XNOR") return 3;
    if (token == "OR" || token == "NOR") return 2;
    return 0;
}

bool isBinaryOperator(const string& token) {
    return token == "AND" || token == "OR" ||
           token == "XOR" || token == "NAND" ||
           token == "NOR" || token == "XNOR";
}
}

bool ExpressionParser::isOperator(const string& token) {
    return token == "AND" || token == "OR" ||
           token == "NOT" || token == "XOR" ||
           token == "NAND" || token == "NOR" ||
           token == "XNOR";
}

vector<string> ExpressionParser::tokenize(const string& expression) const {
    vector<string> tokens;
    size_t i = 0;

    while (i < expression.size()) {
        unsigned char c = static_cast<unsigned char>(expression[i]);

        if (isspace(c)) {
            ++i;
            continue;
        }

        if (isalpha(c) || expression[i] == '_') {
            size_t start = i;

            while (i < expression.size() &&
                   isIdentifierPart(expression[i])) {
                ++i;
            }

            string word = toUpper(expression.substr(start, i - start));

            if (word == "AND" || word == "OR" ||
                word == "NOT" || word == "XOR" ||
                word == "NAND" || word == "NOR" ||
                word == "XNOR") {
                tokens.push_back(word);
            } else {
                tokens.push_back(expression.substr(start, i - start));
            }

            continue;
        }

        if (isdigit(c)) {
            tokens.emplace_back(1, expression[i]);
            ++i;
            continue;
        }

        switch (expression[i]) {
        case '&':
            tokens.push_back("AND");
            ++i;
            if (i < expression.size() && expression[i] == '&') ++i;
            break;

        case '|':
            tokens.push_back("OR");
            ++i;
            if (i < expression.size() && expression[i] == '|') ++i;
            break;

        case '!':
        case '~':
            tokens.push_back("NOT");
            ++i;
            break;

        case '^':
            tokens.push_back("XOR");
            ++i;
            break;

        case '(':
        case ')':
            tokens.emplace_back(1, expression[i]);
            ++i;
            break;

        default:
            throw invalid_argument(
                "Invalid character in expression: " +
                string(1, expression[i]));
        }
    }

    return tokens;
}

vector<string> ExpressionParser::toPostfix(const string& expression) const {
    vector<string> output;
    vector<string> operators;
    vector<string> tokens = tokenize(expression);

    if (tokens.empty()) {
        throw invalid_argument("Expression cannot be empty.");
    }

    bool expectOperand = true;
    int openParentheses = 0;

    for (const string& token : tokens) {
        if (token == "(") {
            if (!expectOperand) {
                throw invalid_argument(
                    "Missing operator before opening parenthesis.");
            }

            operators.push_back(token);
            ++openParentheses;
        }
        else if (token == ")") {
            if (openParentheses == 0) {
                throw invalid_argument(
                    "Closing parenthesis has no matching opening parenthesis.");
            }

            if (expectOperand) {
                throw invalid_argument(
                    "Missing operand before closing parenthesis.");
            }

            while (!operators.empty() && operators.back() != "(") {
                output.push_back(operators.back());
                operators.pop_back();
            }

            if (operators.empty()) {
                throw invalid_argument("Mismatched parentheses.");
            }

            operators.pop_back();
            --openParentheses;

            while (!operators.empty() && operators.back() == "NOT") {
                output.push_back(operators.back());
                operators.pop_back();
            }

            expectOperand = false;
        }
        else if (token == "NOT") {
            if (!expectOperand) {
                throw invalid_argument(
                    "Missing operator before NOT.");
            }

            operators.push_back(token);
        }
        else if (isBinaryOperator(token)) {
            if (expectOperand) {
                throw invalid_argument(
                    "Binary operator is missing a left operand.");
            }

            while (!operators.empty() &&
                   operators.back() != "(" &&
                   precedence(operators.back()) >= precedence(token)) {
                output.push_back(operators.back());
                operators.pop_back();
            }

            operators.push_back(token);
            expectOperand = true;
        }
        else {
            if (!expectOperand) {
                throw invalid_argument(
                    "Missing operator between operands.");
            }

            output.push_back(token);
            expectOperand = false;

            while (!operators.empty() && operators.back() == "NOT") {
                output.push_back(operators.back());
                operators.pop_back();
            }
        }
    }

    if (openParentheses > 0) {
        throw invalid_argument("Missing closing parenthesis.");
    }

    if (expectOperand) {
        throw invalid_argument(
            "Expression cannot end with an operator.");
    }

    while (!operators.empty()) {
        if (operators.back() == "(") {
            throw invalid_argument("Missing closing parenthesis.");
        }

        output.push_back(operators.back());
        operators.pop_back();
    }

    return output;
}

vector<string> ExpressionParser::getVariables(
    const string& expression) const {

    vector<string> variables;
    unordered_set<string> seen;

    for (const string& token : tokenize(expression)) {
        if (token == "(" || token == ")" || isOperator(token)) {
            continue;
        }

        if (seen.insert(token).second) {
            variables.push_back(token);
        }
    }

    return variables;
}