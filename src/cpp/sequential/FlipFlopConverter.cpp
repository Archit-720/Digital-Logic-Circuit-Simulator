#include "FlipFlopConverter.h"

#include <stdexcept>

std::string FlipFlopConverter::typeName(FlipFlopType type)
{
    switch (type) {
        case FlipFlopType::SR: return "SR";
        case FlipFlopType::JK: return "JK";
        case FlipFlopType::D:  return "D";
        case FlipFlopType::T:  return "T";
    }

    return "Unknown";
}

std::string FlipFlopConverter::characteristicEquation(FlipFlopType type)
{
    switch (type) {
        case FlipFlopType::SR:
            return "Q(next) = S + (!R & Q), with S = R = 1 invalid";
        case FlipFlopType::JK:
            return "Q(next) = (J & !Q) | (!K & Q)";
        case FlipFlopType::D:
            return "Q(next) = D";
        case FlipFlopType::T:
            return "Q(next) = T XOR Q";
    }

    return "Unknown";
}

std::string FlipFlopConverter::excitationEquation(FlipFlopType target)
{
    switch (target) {
        case FlipFlopType::SR:
            return "S = !Q & Q(next); R = Q & !Q(next)";
        case FlipFlopType::JK:
            return "J = !Q & Q(next); K = Q & !Q(next)";
        case FlipFlopType::D:
            return "D = Q(next)";
        case FlipFlopType::T:
            return "T = Q XOR Q(next)";
    }

    throw std::invalid_argument("Unknown flip-flop type.");
}

std::string FlipFlopConverter::excitationTableExplanation(FlipFlopType target)
{
    switch (target) {
        case FlipFlopType::SR:
            return
                "SR excitation table:\n"
                "Q Q(next) | S R\n"
                "0    0    | 0 X\n"
                "0    1    | 1 0\n"
                "1    0    | 0 1\n"
                "1    1    | X 0\n"
                "X means either input value is acceptable. S=R=1 is invalid.";
        case FlipFlopType::JK:
            return
                "JK excitation table:\n"
                "Q Q(next) | J K\n"
                "0    0    | 0 X\n"
                "0    1    | 1 X\n"
                "1    0    | X 1\n"
                "1    1    | X 0\n"
                "X means either input value is acceptable.";
        case FlipFlopType::D:
            return
                "D excitation table:\n"
                "Q Q(next) | D\n"
                "0    0    | 0\n"
                "0    1    | 1\n"
                "1    0    | 0\n"
                "1    1    | 1";
        case FlipFlopType::T:
            return
                "T excitation table:\n"
                "Q Q(next) | T\n"
                "0    0    | 0\n"
                "0    1    | 1\n"
                "1    0    | 1\n"
                "1    1    | 0";
    }

    return "Unknown target type.";
}

std::vector<FlipFlopRow> FlipFlopConverter::conversionTable(
    FlipFlopType source,
    FlipFlopType target)
{
    std::vector<FlipFlopRow> rows;

    for (int q = 0; q <= 1; ++q) {
        int combinations = 1;

        switch (source) {
            case FlipFlopType::SR:
            case FlipFlopType::JK:
                combinations = 4;
                break;
            case FlipFlopType::D:
            case FlipFlopType::T:
                combinations = 2;
                break;
        }

        for (int bits = 0; bits < combinations; ++bits) {
            FlipFlopRow row;
            row.q = q;

            int nextQ = q;

            switch (source) {
                case FlipFlopType::SR: {
                    int s = (bits >> 1) & 1;
                    int r = bits & 1;

                    row.sourceS = s;
                    row.sourceR = r;

                    if (s == 1 && r == 1) {
                        row.valid = false;
                    } else if (s == 1) {
                        nextQ = 1;
                    } else if (r == 1) {
                        nextQ = 0;
                    } else {
                        nextQ = q;
                    }

                    break;
                }

                case FlipFlopType::JK: {
                    int j = (bits >> 1) & 1;
                    int k = bits & 1;

                    row.sourceJ = j;
                    row.sourceK = k;

                    if (j == 0 && k == 0)
                        nextQ = q;
                    else if (j == 0 && k == 1)
                        nextQ = 0;
                    else if (j == 1 && k == 0)
                        nextQ = 1;
                    else
                        nextQ = 1 - q;

                    break;
                }

                case FlipFlopType::D:
                    row.sourceD = bits;
                    nextQ = bits;
                    break;

                case FlipFlopType::T:
                    row.sourceT = bits;
                    nextQ = q ^ bits;
                    break;
            }

            row.nextQ = row.valid ? nextQ : -1;

            if (row.valid) {
                switch (target) {
                    case FlipFlopType::SR:
                        row.inputS = (q == 0 && nextQ == 1) ? 1 : 0;
                        row.inputR = (q == 1 && nextQ == 0) ? 1 : 0;
                        break;

                    case FlipFlopType::JK:
                        row.inputJ = (q == 0 && nextQ == 1) ? 1 : 0;
                        row.inputK = (q == 1 && nextQ == 0) ? 1 : 0;
                        break;

                    case FlipFlopType::D:
                        row.inputD = nextQ;
                        break;

                    case FlipFlopType::T:
                        row.inputT = q ^ nextQ;
                        break;
                }
            }

            rows.push_back(row);
        }
    }

    return rows;
}