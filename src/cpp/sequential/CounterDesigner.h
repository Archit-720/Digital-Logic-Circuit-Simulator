#ifndef COUNTER_DESIGNER_H
#define COUNTER_DESIGNER_H

#include "FlipFlopConverter.h"
#include <string>
#include <vector>

enum class CounterMode { Synchronous, Asynchronous };
enum class CounterDirection { Up, Down };

struct CounterState {
    int decimalValue;
    std::string binaryValue;
    int nextDecimalValue;
    std::string nextBinaryValue;

   
    std::vector<std::string> excitationInputs;
};

struct CounterDesign {
    int bitCount = 0;
    CounterMode mode = CounterMode::Synchronous;
    CounterDirection direction = CounterDirection::Up;
    FlipFlopType flipFlop = FlipFlopType::JK;

    std::vector<CounterState> states;
    std::vector<std::string> notes;
};

class CounterDesigner {
public:
    static CounterDesign design(
        int bitCount,
        CounterMode mode,
        CounterDirection direction,
        FlipFlopType flipFlop = FlipFlopType::JK
    );

    static std::string toBinary(int value, int bitCount);

private:
    static std::string excitation(
        int currentBit,
        int nextBit,
        FlipFlopType flipFlop
    );
};

#endif