#include "KMapSolver.h"
#include "../expression/BooleanSimplifier.h"

#include <sstream>
#include <algorithm>
#include <cctype>

namespace {
bool termMatches(const std::string& term, int value, int variableCount)
{
    for (size_t p = 0; p < term.size();) {
        if (term[p] == ' ' || term[p] == '&' || term[p] == '*') {
            ++p;
            continue;
        }

        bool negated = false;

        if (term[p] == '!') {
            negated = true;
            ++p;
        }

        if (p >= term.size())
            break;

        char variable = static_cast<char>(
            std::toupper(static_cast<unsigned char>(term[p]))
        );
        ++p;

        if (variable < 'A' || variable >= 'A' + variableCount)
            continue;

        int position = variable - 'A';
        bool bit = (value >> (variableCount - 1 - position)) & 1;

        if (negated ? bit : !bit)
            return false;
    }

    return true;
}

bool expressionMatches(const std::string& expression,
                       int value,
                       int variableCount)
{
    if (expression == "1")
        return true;

    if (expression == "0" || expression.empty())
        return false;

    size_t start = 0;

    while (start < expression.size()) {
        size_t open = expression.find('(', start);

        if (open == std::string::npos)
            break;

        size_t close = expression.find(')', open + 1);

        if (close == std::string::npos)
            break;

        std::string term = expression.substr(
            open + 1, close - open - 1
        );

        if (termMatches(term, value, variableCount))
            return true;

        start = close + 1;
    }

    return false;
}

std::string binaryString(int value, int bits)
{
    std::string result;

    for (int i = bits - 1; i >= 0; --i)
        result += ((value >> i) & 1) ? '1' : '0';

    return result.empty() ? "-" : result;
}
}

KMapResult KMapSolver::solve(const KMapData& data)
{
    KMapResult result;

    const int n = data.variableCount();
    const int limit = 1 << n;

    result.expression = BooleanSimplifier::simplify(
        n, data.minterms(), data.dontCares()
    );

    std::ostringstream inputStep;
    inputStep << "Step 1: Read the function.\n"
              << "Variables: ";

    for (int i = 0; i < n; ++i) {
        if (i > 0)
            inputStep << ", ";

        inputStep << static_cast<char>('A' + i);
    }

    inputStep << "\nMinterms: ";

    if (data.minterms().empty()) {
        inputStep << "none";
    } else {
        for (size_t i = 0; i < data.minterms().size(); ++i) {
            if (i > 0)
                inputStep << ", ";

            inputStep << data.minterms()[i];
        }
    }

    inputStep << "\nDon't-cares: ";

    if (data.dontCares().empty()) {
        inputStep << "none";
    } else {
        for (size_t i = 0; i < data.dontCares().size(); ++i) {
            if (i > 0)
                inputStep << ", ";

            inputStep << data.dontCares()[i];
        }
    }

    result.steps.push_back(inputStep.str());

    result.steps.push_back(
        "Step 2: Simplify the function using the Boolean simplifier. "
        "Don't-care cells may help form larger groups, but they do not "
        "have to be covered."
    );

    if (result.expression == "0" || result.expression == "1") {
        KMapGroup group;
        group.implicant = result.expression;

        for (int value = 0; value < limit; ++value) {
            if (data.cellValue(value) != 0)
                group.cells.push_back(value);
        }

        group.explanation = result.expression == "1"
            ? "The function is constant 1."
            : "The function is constant 0.";

        result.groups.push_back(group);
    } else {
        size_t start = 0;

        while (start < result.expression.size()) {
            size_t open = result.expression.find('(', start);

            if (open == std::string::npos)
                break;

            size_t close = result.expression.find(')', open + 1);

            if (close == std::string::npos)
                break;

            std::string term = result.expression.substr(
                open + 1, close - open - 1
            );

            KMapGroup group;
            group.implicant = term;

            for (int value = 0; value < limit; ++value) {
                if (data.cellValue(value) != 0 &&
                    termMatches(term, value, n)) {
                    group.cells.push_back(value);
                }
            }

            std::ostringstream explanation;
            explanation << "Group " << result.groups.size() + 1
                        << ": term ";

            if (term.empty())
                explanation << "(empty)";
            else
                explanation << term;

            explanation << " covers ";

            if (group.cells.empty()) {
                explanation << "no required or don't-care cells.";
            } else {
                for (size_t i = 0; i < group.cells.size(); ++i) {
                    if (i > 0)
                        explanation << ", ";

                    explanation << group.cells[i];

                    if (data.isDontCare(group.cells[i]))
                        explanation << "(X)";
                }

                explanation << ".";
            }

            group.explanation = explanation.str();
            result.groups.push_back(group);

            start = close + 1;
        }
    }

    std::ostringstream groupsStep;
    groupsStep << "Step 3: Interpret the groups.\n";

    if (result.groups.empty()) {
        groupsStep << "No product-term groups were extracted from the "
                      "simplified expression. Check the expression format.";
    } else {
        for (const auto& group : result.groups)
            groupsStep << group.explanation << '\n';
    }

    result.steps.push_back(groupsStep.str());

    std::ostringstream finalStep;
    finalStep << "Step 4: Simplified expression = "
              << result.expression;

    result.steps.push_back(finalStep.str());

    result.verified = true;

    for (int value = 0; value < limit; ++value) {
        bool actual = expressionMatches(result.expression, value, n);
        bool required = data.isMinterm(value);

        if (required && !actual) {
            result.verified = false;
            break;
        }

        if (data.cellValue(value) == 0 && actual) {
            result.verified = false;
            break;
        }
    }

    result.steps.push_back(
        result.verified
            ? "Step 5: Verification passed. Every minterm evaluates to 1, "
              "and every zero-cell evaluates to 0. Don't-care cells may "
              "evaluate to either value."
            : "Step 5: Verification failed. The simplified expression "
              "does not match the supplied function."
    );

    return result;
}