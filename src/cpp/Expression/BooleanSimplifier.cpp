#include "BooleanSimplifier.h"

#include <algorithm>
#include <functional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <utility>

namespace
{
struct Implicant
{
    std::string bits;
    std::set<int> covered;
    bool used = false;
};

int countLiterals(const std::string& bits)
{
    return static_cast<int>(
        std::count_if(
            bits.begin(),
            bits.end(),
            [](char bit)
            {
                return bit != '-';
            }
        )
    );
}

bool combine(
    const std::string& a,
    const std::string& b,
    std::string& result
)
{
    int differences = 0;
    result = a;

    for (size_t i = 0; i < a.size(); ++i)
    {
        if (a[i] == b[i])
            continue;

        if (a[i] == '-' || b[i] == '-')
            return false;

        ++differences;
        result[i] = '-';
    }

    return differences == 1;
}

std::string toBinary(int value, int variableCount)
{
    std::string bits(variableCount, '0');

    for (int i = 0; i < variableCount; ++i)
    {
        bits[variableCount - 1 - i] =
            ((value >> i) & 1) ? '1' : '0';
    }

    return bits;
}

std::string termText(const std::string& bits)
{
    std::string result;

    for (size_t i = 0; i < bits.size(); ++i)
    {
        if (bits[i] == '-')
            continue;

        if (bits[i] == '0')
            result += '!';

        result += static_cast<char>('A' + i);
    }

    return result.empty() ? "1" : result;
}

std::string expressionText(
    const std::vector<Implicant>& primes,
    const std::vector<int>& selected
)
{
    if (selected.empty())
        return "0";

    for (int index : selected)
    {
        if (primes[index].bits.find_first_not_of('-')
            == std::string::npos)
        {
            return "1";
        }
    }

    std::ostringstream out;

    for (size_t i = 0; i < selected.size(); ++i)
    {
        if (i != 0)
            out << " | ";

        out << "(" << termText(primes[selected[i]].bits) << ")";
    }

    return out.str();
}
}

BooleanSimplificationResult BooleanSimplifier::simplifyDetailed(
    int variableCount,
    const std::vector<int>& minterms,
    const std::vector<int>& dontCares
)
{
    if (variableCount < 1 || variableCount > 8)
    {
        throw std::invalid_argument(
            "Variable count must be between 1 and 8."
        );
    }

    const int limit = 1 << variableCount;

    std::set<int> target(minterms.begin(), minterms.end());
    std::set<int> dontCareSet(dontCares.begin(), dontCares.end());

    for (int value : target)
    {
        if (value < 0 || value >= limit)
            throw std::invalid_argument("Minterm out of range.");
    }

    for (int value : dontCareSet)
    {
        if (value < 0 || value >= limit)
            throw std::invalid_argument("Don't-care out of range.");
    }

    for (int value : target)
        dontCareSet.erase(value);

    BooleanSimplificationResult result;

    std::ostringstream original;
    original << "F(";

    for (int i = 0; i < variableCount; ++i)
    {
        if (i != 0)
            original << ",";

        original << static_cast<char>('A' + i);
    }

    original << ") = ";

    if (target.empty())
    {
        result.originalFunction = original.str() + "0";
        result.simplifiedExpression = "0";
        result.steps.push_back("No minterms were supplied.");
        result.steps.push_back("The function is therefore constant 0.");
        return result;
    }

    original << "Sigma m(";

    bool first = true;

    for (int value : target)
    {
        if (!first)
            original << ",";

        original << value;
        first = false;
    }

    original << ")";
    result.originalFunction = original.str();

    if (!dontCareSet.empty())
    {
        std::ostringstream dc;
        dc << "Don't-care terms: ";

        first = true;

        for (int value : dontCareSet)
        {
            if (!first)
                dc << ", ";

            dc << value;
            first = false;
        }

        result.steps.push_back(dc.str());
    }

    result.steps.push_back(
        "Step 1: Convert minterms and don't-care terms into binary."
    );

    std::set<int> all = target;
    all.insert(dontCareSet.begin(), dontCareSet.end());

    std::vector<Implicant> current;

    for (int value : all)
    {
        std::string bits = toBinary(value, variableCount);

        std::ostringstream line;
        line << "m" << value << " = " << bits;

        if (!target.count(value))
            line << " (don't-care)";

        result.steps.push_back(line.str());
        current.push_back({bits, {value}, false});
    }

    std::vector<Implicant> primes;
    int round = 1;

    while (!current.empty())
    {
        result.steps.push_back(
            "Step " + std::to_string(round + 1) +
            ": Combine terms differing in exactly one bit."
        );

        std::vector<Implicant> next;
        std::set<std::string> seen;
        bool anyCombination = false;

        for (auto& item : current)
            item.used = false;

        for (size_t i = 0; i < current.size(); ++i)
        {
            for (size_t j = i + 1; j < current.size(); ++j)
            {
                std::string combined;

                if (!combine(
                    current[i].bits,
                    current[j].bits,
                    combined
                ))
                {
                    continue;
                }

                anyCombination = true;
                current[i].used = true;
                current[j].used = true;

                std::set<int> covered = current[i].covered;
                covered.insert(
                    current[j].covered.begin(),
                    current[j].covered.end()
                );

                std::ostringstream line;
                line << current[i].bits
                     << " + "
                     << current[j].bits
                     << " -> "
                     << combined;

                result.steps.push_back(line.str());

                if (seen.insert(combined).second)
                {
                    next.push_back({combined, covered, false});
                }
                else
                {
                    for (auto& existing : next)
                    {
                        if (existing.bits == combined)
                        {
                            existing.covered.insert(
                                covered.begin(),
                                covered.end()
                            );
                            break;
                        }
                    }
                }
            }
        }

        for (const auto& implicant : current)
        {
            if (!implicant.used)
                primes.push_back(implicant);
        }

        if (!anyCombination)
            break;

        current = std::move(next);
        ++round;
    }

    std::sort(
        primes.begin(),
        primes.end(),
        [](const Implicant& a, const Implicant& b)
        {
            return a.bits < b.bits;
        }
    );

    std::vector<Implicant> uniquePrimes;

    for (const auto& prime : primes)
    {
        if (!uniquePrimes.empty() &&
            uniquePrimes.back().bits == prime.bits)
        {
            uniquePrimes.back().covered.insert(
                prime.covered.begin(),
                prime.covered.end()
            );
        }
        else
        {
            uniquePrimes.push_back(prime);
        }
    }

    primes = std::move(uniquePrimes);

    result.steps.push_back("Prime implicants:");

    for (const auto& prime : primes)
    {
        result.steps.push_back(
            prime.bits + " -> " + termText(prime.bits)
        );
    }

    std::vector<int> selected;
    std::vector<int> bestSelection;

    int bestLiteralCount = std::numeric_limits<int>::max();
    int bestTermCount = std::numeric_limits<int>::max();

    std::function<void(size_t, const std::set<int>&, int)> searchCover;

    searchCover = [&](size_t start, const std::set<int>& covered, int literals)
    {
        bool complete = true;

        for (int value : target)
        {
            if (!covered.count(value))
            {
                complete = false;
                break;
            }
        }

        if (complete)
        {
            int termCount = static_cast<int>(selected.size());

            if (literals < bestLiteralCount ||
                (literals == bestLiteralCount &&
                 termCount < bestTermCount))
            {
                bestLiteralCount = literals;
                bestTermCount = termCount;
                bestSelection = selected;
            }

            return;
        }

        if (start >= primes.size())
            return;

        if (literals > bestLiteralCount)
            return;

        for (size_t i = start; i < primes.size(); ++i)
        {
            bool addsCoverage = false;

            for (int value : primes[i].covered)
            {
                if (target.count(value) && !covered.count(value))
                {
                    addsCoverage = true;
                    break;
                }
            }

            if (!addsCoverage)
                continue;

            int nextLiterals = literals + countLiterals(primes[i].bits);

            if (nextLiterals > bestLiteralCount)
                continue;

            std::set<int> nextCovered = covered;

            for (int value : primes[i].covered)
            {
                if (target.count(value))
                    nextCovered.insert(value);
            }

            selected.push_back(static_cast<int>(i));
            searchCover(i + 1, nextCovered, nextLiterals);
            selected.pop_back();
        }
    };

    searchCover(0, {}, 0);

    if (bestSelection.empty())
        throw std::runtime_error("Could not cover all minterms.");

    result.simplifiedExpression =
        expressionText(primes, bestSelection);

    result.steps.push_back(
        "Select prime implicants that cover every required minterm."
    );

    for (int index : bestSelection)
    {
        result.steps.push_back(
            "Selected: " + termText(primes[index].bits)
        );
    }

    result.steps.push_back(
        "Final simplified expression: " +
        result.simplifiedExpression
    );

    return result;
}

std::string BooleanSimplifier::simplify(
    int variableCount,
    const std::vector<int>& minterms,
    const std::vector<int>& dontCares
)
{
    return simplifyDetailed(
        variableCount,
        minterms,
        dontCares
    ).simplifiedExpression;
}