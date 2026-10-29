#ifndef TRUTH_TABLE_H
#define TRUTH_TABLE_H

#include <string>
#include <vector>

struct TruthTableRow
{
    std::vector<int> inputs;
    int output = 0;
    std::vector<int> intermediateValues;
};

struct TruthTableResult
{
    std::vector<std::string> intermediateExpressions;
    std::vector<TruthTableRow> rows;
};

class TruthTable
{
public:
    static std::vector<TruthTableRow> generate(
        int variableCount,
        const std::string& expression
    );

    static TruthTableResult generateDetailed(
        int variableCount,
        const std::string& expression
    );

    static bool evaluate(
        const std::string& expression,
        const std::vector<int>& values
    );
};

#endif