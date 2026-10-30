#include "CounterDesigner.h"
#include <stdexcept>
#include <sstream>

std::string CounterDesigner::toBinary(int value, int bitCount)
{
    std::string result(bitCount, '0');

    for (int i = 0; i < bitCount; ++i) {
        if ((value >> i) & 1) {
            result[bitCount - 1 - i] = '1';
        }
    }

    return result;
}

std::string CounterDesigner::excitation(
    int currentBit,
    int nextBit,
    FlipFlopType flipFlop)
{
    if (flipFlop == FlipFlopType::D) {
        return "D=" + std::to_string(nextBit);
    }

    if (flipFlop == FlipFlopType::T) {
        return "T=" + std::to_string(currentBit ^ nextBit);
    }

    if (flipFlop == FlipFlopType::JK) {
        if (currentBit == 0 && nextBit == 0) return "J=0,K=X";
        if (currentBit == 0 && nextBit == 1) return "J=1,K=X";
        if (currentBit == 1 && nextBit == 0) return "J=X,K=1";
        return "J=X,K=0";
    }

    if (flipFlop == FlipFlopType::SR) {
        if (currentBit == 0 && nextBit == 0) return "S=0,R=X";
        if (currentBit == 0 && nextBit == 1) return "S=1,R=0";
        if (currentBit == 1 && nextBit == 0) return "S=0,R=1";
        return "S=X,R=0";
    }

    return "Unknown";
}

CounterDesign CounterDesigner::design(
    int bitCount,
    CounterMode mode,
    CounterDirection direction,
    FlipFlopType flipFlop)
{
    if (bitCount < 1 || bitCount > 12) {
        throw std::invalid_argument(
            "Counter bit count must be between 1 and 12."
        );
    }

    CounterDesign design;
    design.bitCount = bitCount;
    design.mode = mode;
    design.direction = direction;
    design.flipFlop = flipFlop;

    const int total = 1 << bitCount;

    for (int value = 0; value < total; ++value) {
        int next;

        if (direction == CounterDirection::Up) {
            next = (value + 1) % total;
        } else {
            next = (value - 1 + total) % total;
        }

        CounterState state;
        state.decimalValue = value;
        state.binaryValue = toBinary(value, bitCount);
        state.nextDecimalValue = next;
        state.nextBinaryValue = toBinary(next, bitCount);

        for (int bit = bitCount - 1; bit >= 0; --bit) {
            int currentBit = (value >> bit) & 1;
            int nextBit = (next >> bit) & 1;

            state.excitationInputs.push_back(
                excitation(currentBit, nextBit, flipFlop)
            );
        }

        design.states.push_back(state);
    }

    if (mode == CounterMode::Synchronous) {
        design.notes.push_back(
            "Synchronous counter: all flip-flops share a common clock."
        );
    } else {
        design.notes.push_back(
            "Asynchronous counter: clock transitions ripple through stages. "
            "The state table alone does not specify the complete clock wiring."
        );
    }

    if (direction == CounterDirection::Up) {
        design.notes.push_back(
            "Up counter: the state increments by one and wraps from "
            + std::to_string(total - 1) + " to 0."
        );
    } else {
        design.notes.push_back(
            "Down counter: the state decrements by one and wraps from "
            "0 to " + std::to_string(total - 1) + "."
        );
    }

    design.notes.push_back(
        "Number of flip-flops: " + std::to_string(bitCount) + "."
    );

    design.notes.push_back(
        "Total possible states: " + std::to_string(total) + "."
    );

    if (flipFlop == FlipFlopType::D) {
        design.notes.push_back(
            "D flip-flop: D equals the required next-state bit."
        );
    } else if (flipFlop == FlipFlopType::T) {
        design.notes.push_back(
            "T flip-flop: T=0 holds the state; T=1 toggles the state."
        );
    } else if (flipFlop == FlipFlopType::JK) {
        design.notes.push_back(
            "JK flip-flop: J sets the state and K resets it; X denotes "
            "a don't-care input."
        );
    } else if (flipFlop == FlipFlopType::SR) {
        design.notes.push_back(
            "SR flip-flop: S sets the state and R resets it. "
            "The forbidden S=1,R=1 combination is avoided."
        );
    }

    design.notes.push_back(
        "These are excitation requirements for each state transition. "
        "They are not minimized Boolean equations or a synthesized circuit."
    );

    return design;
}