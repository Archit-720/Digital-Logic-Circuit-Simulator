#include "TruthTable.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
class Parser
{
public:
    Parser(
        const std::string& expression,
        const std::vector<int>& values,
        std::vector<std::string>* labels = nullptr,
        std::vector<int>* intermediateValues = nullptr
    )
        : text(expression),
          inputs(values),
          labels_(labels),
          intermediateValues_(intermediateValues)
    {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char c)
            {
                return static_cast<char>(std::toupper(c));
            }
        );
    }

    bool parse()
    {
        pos = 0;

        bool result = parseOr();
        skip();

        if (pos != text.size())
            throw std::runtime_error(
                "Unexpected expression text near: " + text.substr(pos)
            );

        return result;
    }

private:
    std::string text;
    const std::vector<int>& inputs;
    std::vector<std::string>* labels_;
    std::vector<int>* intermediateValues_;
    size_t pos = 0;

    void skip()
    {
        while (
            pos < text.size() &&
            std::isspace(static_cast<unsigned char>(text[pos]))
        )
        {
            ++pos;
        }
    }

    bool take(char c)
    {
        skip();

        if (pos < text.size() && text[pos] == c)
        {
            ++pos;
            return true;
        }

        return false;
    }

    bool takeWord(const std::string& word)
    {
        skip();

        if (text.compare(pos, word.size(), word) != 0)
            return false;

        size_t end = pos + word.size();

        if (
            end < text.size() &&
            (
                std::isalnum(static_cast<unsigned char>(text[end])) ||
                text[end] == '_'
            )
        )
        {
            return false;
        }

        if (
            pos > 0 &&
            (
                std::isalnum(static_cast<unsigned char>(text[pos - 1])) ||
                text[pos - 1] == '_'
            )
        )
        {
            return false;
        }

        pos = end;
        return true;
    }

    void record(size_t start, bool value)
    {
        if (!labels_ || !intermediateValues_)
            return;

        std::string label = text.substr(start, pos - start);

        if (label.empty())
            label = "Intermediate";

        labels_->push_back(label);
        intermediateValues_->push_back(value ? 1 : 0);
    }

    bool parseOr()
    {
        size_t start = pos;
        bool value = parseXor();

        while (true)
        {
            bool isNor = false;
            bool found = false;

            if (take('|') || take('+') || takeWord("OR"))
            {
                found = true;
            }
            else if (takeWord("NOR"))
            {
                found = true;
                isNor = true;
            }

            if (!found)
                return value;

            bool right = parseXor();

            value = isNor
                ? !(value || right)
                : (value || right);

            record(start, value);
        }
    }

    bool parseXor()
    {
        size_t start = pos;
        bool value = parseAnd();

        while (true)
        {
            bool isXnor = false;
            bool found = false;

            if (take('^') || takeWord("XOR"))
            {
                found = true;
            }
            else if (takeWord("XNOR"))
            {
                found = true;
                isXnor = true;
            }

            if (!found)
                return value;

            bool right = parseAnd();

            value = isXnor
                ? (value == right)
                : (value != right);

            record(start, value);
        }
    }

    bool parseAnd()
    {
        size_t start = pos;
        bool value = parseUnary();

        while (true)
        {
            bool isNand = false;
            bool found = false;

            if (take('&') || take('*') || takeWord("AND"))
            {
                found = true;
            }
            else if (takeWord("NAND"))
            {
                found = true;
                isNand = true;
            }

            if (!found)
                return value;

            bool right = parseUnary();

            value = isNand
                ? !(value && right)
                : (value && right);

            record(start, value);
        }
    }

    bool parseUnary()
    {
        skip();
        size_t start = pos;

        if (take('!') || take('~') || takeWord("NOT"))
        {
            bool value = !parseUnary();
            record(start, value);
            return value;
        }

        if (take('('))
        {
            bool value = parseOr();

            if (!take(')'))
            {
                throw std::runtime_error(
                    "Missing closing parenthesis."
                );
            }

            return value;
        }

        skip();

        if (pos >= text.size())
        {
            throw std::runtime_error(
                "Unexpected end of expression."
            );
        }

        char c = text[pos++];

        if (c == '0')
            return false;

        if (c == '1')
            return true;

        if (c >= 'A' && c <= 'Z')
        {
            int index = c - 'A';

            if (index >= static_cast<int>(inputs.size()))
            {
                throw std::runtime_error(
                    std::string("Variable ") + c +
                    " has no supplied input value. Increase the variable count."
                );
            }

            return inputs[index] != 0;
        }

        throw std::runtime_error(
            "Unsupported expression syntax. Use variables A-Z, constants "
            "0 and 1, NOT, AND, OR, XOR, NAND, NOR, XNOR, and parentheses."
        );
    }
};
}

bool TruthTable::evaluate(
    const std::string& expression,
    const std::vector<int>& values
)
{
    Parser parser(expression, values);
    return parser.parse();
}

std::vector<TruthTableRow> TruthTable::generate(
    int variableCount,
    const std::string& expression
)
{
    return generateDetailed(variableCount, expression).rows;
}

TruthTableResult TruthTable::generateDetailed(
    int variableCount,
    const std::string& expression
)
{
    if (variableCount < 1 || variableCount > 16)
    {
        throw std::invalid_argument(
            "Variable count must be between 1 and 16."
        );
    }

    if (expression.empty())
    {
        throw std::invalid_argument(
            "Please enter a Boolean expression."
        );
    }

    TruthTableResult result;
    const unsigned total = 1u << variableCount;

    for (unsigned number = 0; number < total; ++number)
    {
        TruthTableRow row;

        for (int bit = variableCount - 1; bit >= 0; --bit)
        {
            row.inputs.push_back(
                static_cast<int>((number >> bit) & 1u)
            );
        }

        std::vector<std::string> labels;
        std::vector<int> intermediateValues;

        Parser parser(
            expression,
            row.inputs,
            &labels,
            &intermediateValues
        );

        row.output = parser.parse() ? 1 : 0;
        row.intermediateValues = intermediateValues;

        if (number == 0)
        {
            result.intermediateExpressions = labels;
        }
        else if (labels != result.intermediateExpressions)
        {
            throw std::runtime_error(
                "The expression produced inconsistent intermediate columns."
            );
        }

        result.rows.push_back(row);
    }

    return result;
}