#ifndef FLIP_FLOP_CONVERTER_H
#define FLIP_FLOP_CONVERTER_H

#include <string>
#include <vector>

enum class FlipFlopType { SR, JK, D, T };

struct FlipFlopRow {
    int q = 0;
    int nextQ = 0;

    int inputS = -1;
    int inputR = -1;
    int inputJ = -1;
    int inputK = -1;
    int inputD = -1;
    int inputT = -1;

    int sourceS = -1;
    int sourceR = -1;
    int sourceJ = -1;
    int sourceK = -1;
    int sourceD = -1;
    int sourceT = -1;

    bool valid = true;
};

class FlipFlopConverter {
public:
    static std::string typeName(FlipFlopType type);

    static std::vector<FlipFlopRow> conversionTable(
        FlipFlopType source,
        FlipFlopType target
    );

    static std::string characteristicEquation(FlipFlopType type);
    static std::string excitationEquation(FlipFlopType target);
    static std::string excitationTableExplanation(FlipFlopType target);
};

#endif