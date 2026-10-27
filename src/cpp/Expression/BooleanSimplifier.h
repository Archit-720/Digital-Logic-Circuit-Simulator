#ifndef BOOLEAN_SIMPLIFIER_H
#define BOOLEAN_SIMPLIFIER_H

#include <string>
#include <vector>

struct BooleanSimplificationResult
{
    std::string originalFunction;
    std::string simplifiedExpression;
    std::vector<std::string> steps;
};

class BooleanSimplifier
{
public:
    static std::string simplify(
        int variableCount,
        const std::vector<int>& minterms,
        const std::vector<int>& dontCares = {}
    );

    static BooleanSimplificationResult simplifyDetailed(
        int variableCount,
        const std::vector<int>& minterms,
        const std::vector<int>& dontCares = {}
    );
};

#endif