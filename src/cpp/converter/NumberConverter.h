#ifndef NUMBER_CONVERTER_H
#define NUMBER_CONVERTER_H

#include <string>

class NumberConverter {
public:
    // Converts a non-negative whole number from one base to another.
    // Supported bases are 2 through 16.
    std::string convert(const std::string& number, int fromBase, int toBase) const;

private:
    unsigned long long toDecimal(const std::string& number, int base) const;
    int charToDigit(char character) const;
    char digitToChar(int digit) const;
};

#endif
