#include "NumberConverter.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

int NumberConverter::charToDigit(char character) const {
    if (character >= '0' && character <= '9') {
        return character - '0';
    }

    if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    }

    if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    }

    return -1;
}

char NumberConverter::digitToChar(int digit) const {
    if (digit >= 0 && digit <= 9) {
        return static_cast<char>('0' + digit);
    }

    if (digit >= 10 && digit <= 15) {
        return static_cast<char>('A' + digit - 10);
    }

    throw std::invalid_argument("Digit is outside the supported range.");
}

unsigned long long NumberConverter::toDecimal(
    const std::string& number, int base) const {
    if (number.empty()) {
        throw std::invalid_argument("The number cannot be empty.");
    }

    unsigned long long value = 0;
    const unsigned long long maximum =
        std::numeric_limits<unsigned long long>::max();

    for (char character : number) {
        int digit = charToDigit(character);

        if (digit < 0 || digit >= base) {
            throw std::invalid_argument("The number contains a digit that is invalid for this base.");
        }

        if (value > (maximum - static_cast<unsigned long long>(digit)) /
                        static_cast<unsigned long long>(base)) {
            throw std::out_of_range("The number is too large to convert.");
        }

        value = value * static_cast<unsigned long long>(base) +
                static_cast<unsigned long long>(digit);
    }

    return value;
}

std::string NumberConverter::convert(
    const std::string& number, int fromBase, int toBase) const {
    if (fromBase < 2 || fromBase > 16 || toBase < 2 || toBase > 16) {
        throw std::invalid_argument("Supported bases are from 2 to 16.");
    }

    unsigned long long value = toDecimal(number, fromBase);

    if (value == 0) {
        return "0";
    }

    std::string result;

    while (value > 0) {
        int digit = static_cast<int>(value % static_cast<unsigned long long>(toBase));
        result += digitToChar(digit);
        value /= static_cast<unsigned long long>(toBase);
    }

    std::reverse(result.begin(), result.end());
    return result;
}
