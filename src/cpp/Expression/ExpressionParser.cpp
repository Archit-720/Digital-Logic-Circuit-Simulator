#include "ExpressionParser.h"

#include <cctype>
#include <stdexcept>

namespace {
std::string toUpper(const std::string& text) {
    std::string result = text;
    for (char& character : result) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}

bool isIdentifierStart(char character) {
    unsigned char value = static_cast<unsigned char>(character);
    return std::isalpha(value) || character == '_';
}

bool isIdentifierPart(char character) {
    unsigned char value = static_cast<unsigned char>(character);
    return std::isalnum(value) || character == '_';
}

int precedence(const std::string& operation) {
    if (operation == "NOT") return 4;
    if (operation == "AND") return 3;
    if (operation == "XOR") return 2;
    if (operation == "OR") return 1;
    return 0;
}

bool isBinaryOperator(const std::string& token) {
    return token == "AND" || token == "OR" || token == "XOR";
}
}

bool ExpressionParser::isOperator(const std::string& token) {
    return token == "AND" || token == "OR" || token == "NOT" || token == "XOR";
}

std::vector<std::string> ExpressionParser::tokenize(const std::string& expression) const {
    std::vector<std::string> tokens;
    std::size_t position = 0;

    while (position < expression.size()) {
        char character = expression[position];

        if (std::isspace(static_cast<unsigned char>(character))) {
            position++;
            continue;
        }

        if (isIdentifierStart(character)) {
            std::size_t start = position;
            while (position < expression.size() && isIdentifierPart(expression[position])) {
                position++;
            }

            std::string word = expression.substr(start, position - start);
            std::string upperWord = toUpper(word);

            if (upperWord == "AND" || upperWord == "OR" ||
                upperWord == "NOT" || upperWord == "XOR") {
                tokens.push_back(upperWord);
            } else {
                tokens.push_back(word);
            }
            continue;
        }

        if (character == '(' || character == ')') {
            tokens.push_back(std::string(1, character));
            position++;
            continue;
        }

        if (character == '&') {
            tokens.push_back("AND");
            position++;
            if (position < expression.size() && expression[position] == '&') {
                position++;
            }
            continue;
        }

        if (character == '|') {
            tokens.push_back("OR");
            position++;
            if (position < expression.size() && expression[position] == '|') {
                position++;
            }
            continue;
        }

        if (character == '!' || character == '~') {
            tokens.push_back("NOT");
            position++;
            continue;
        }

        if (character == '^') {
            tokens.push_back("XOR");
            position++;
            continue;
        }

        throw std::invalid_argument("Invalid character in expression: " + std::string(1, character));
    }

    return tokens;
}

std::vector<std::string> ExpressionParser::toPostfix(const std::string& expression) const {
    std::vector<std::string> tokens = tokenize(expression);
    if (tokens.empty()) {
        throw std::invalid_argument("Expression cannot be empty.");
    }

    std::vector<std::string> output;
    std::vector<std::string> operators;
    bool expectOperand = true;

    for (const std::string& token : tokens) {
        if (token == "(") {
            if (!expectOperand) {
                throw std::invalid_argument("Missing operator before '('.");
            }
            operators.push_back(token);
        } else if (token == ")") {
            if (expectOperand) {
                throw std::invalid_argument("Missing operand before ')'.");
            }

            bool foundOpeningParenthesis = false;
            while (!operators.empty()) {
                if (operators.back() == "(") {
                    operators.pop_back();
                    foundOpeningParenthesis = true;
                    break;
                }
                output.push_back(operators.back());
                operators.pop_back();
            }

            if (!foundOpeningParenthesis) {
                throw std::invalid_argument("Unmatched closing parenthesis.");
            }
            expectOperand = false;
        } else if (token == "NOT") {
            if (!expectOperand) {
                throw std::invalid_argument("NOT must appear before an operand.");
            }
            operators.push_back(token);
        } else if (isBinaryOperator(token)) {
            if (expectOperand) {
                throw std::invalid_argument("Binary operator is missing a left operand.");
            }

            while (!operators.empty() && operators.back() != "(") {
                int topPrecedence = precedence(operators.back());
                int currentPrecedence = precedence(token);

                if (topPrecedence >= currentPrecedence) {
                    output.push_back(operators.back());
                    operators.pop_back();
                } else {
                    break;
                }
            }

            operators.push_back(token);
            expectOperand = true;
        } else {
            if (!expectOperand) {
                throw std::invalid_argument("Missing operator between operands.");
            }
            output.push_back(token);
            expectOperand = false;
        }
    }

    if (expectOperand) {
        throw std::invalid_argument("Expression ends where an operand is required.");
    }

    while (!operators.empty()) {
        if (operators.back() == "(") {
            throw std::invalid_argument("Unmatched opening parenthesis.");
        }
        output.push_back(operators.back());
        operators.pop_back();
    }

    return output;
}

std::vector<std::string> ExpressionParser::getVariables(const std::string& expression) const {
    std::vector<std::string> tokens = tokenize(expression);
    std::vector<std::string> variables;

    for (const std::string& token : tokens) {
        if (token == "(" || token == ")" || isOperator(token)) {
            continue;
        }

        bool alreadyAdded = false;
        for (const std::string& variable : variables) {
            if (variable == token) {
                alreadyAdded = true;
                break;
            }
        }

        if (!alreadyAdded) {
            variables.push_back(token);
        }
    }

    return variables;
}
