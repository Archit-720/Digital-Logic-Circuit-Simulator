#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <vector>

class ExpressionParser {
public:
    std::vector<std::string> tokenize(const std::string& expression) const;
    std::vector<std::string> toPostfix(const std::string& expression) const;
    std::vector<std::string> getVariables(const std::string& expression) const;

    static bool isOperator(const std::string& token);
};

#endif
