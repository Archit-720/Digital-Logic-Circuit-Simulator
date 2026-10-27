#include "SequentialGates.h"

#include <stdexcept>

SequentialComponent::SequentialComponent(int initialQ) {
    if (initialQ != 0 && initialQ != 1) {
        throw std::invalid_argument("Initial Q must be 0 or 1.");
    }

    current_Q = initialQ;
    next_Q = initialQ;
}

void SequentialComponent::validateInputs(const std::vector<int>& inputs, int expectedCount) const {
    if (static_cast<int>(inputs.size()) != expectedCount) {
        throw std::invalid_argument("Incorrect number of inputs for this flip-flop.");
    }

    for (int value : inputs) {
        if (value != 0 && value != 1) {
            throw std::invalid_argument("Flip-flop inputs must be 0 or 1.");
        }
    }
}

void SequentialComponent::clockTick() {
    current_Q = next_Q;
}

int SequentialComponent::getQ() const {
    return current_Q;
}

int SequentialComponent::getNotQ() const {
    return 1 - current_Q;
}

SRFlipFlop::SRFlipFlop(int initialQ) : SequentialComponent(initialQ) {}

void SRFlipFlop::calculateNextState(const std::vector<int>& inputs) {
    validateInputs(inputs, 2);

    int set = inputs[0];
    int reset = inputs[1];

    if (set == 1 && reset == 1) {
        throw std::invalid_argument("S and R cannot both be 1 for this SR flip-flop.");
    }

    if (set == 1) {
        next_Q = 1;
    } else if (reset == 1) {
        next_Q = 0;
    } else {
        next_Q = current_Q;
    }
}

JKFlipFlop::JKFlipFlop(int initialQ) : SequentialComponent(initialQ) {}

void JKFlipFlop::calculateNextState(const std::vector<int>& inputs) {
    validateInputs(inputs, 2);

    int j = inputs[0];
    int k = inputs[1];

    if (j == 0 && k == 0) {
        next_Q = current_Q;
    } else if (j == 0 && k == 1) {
        next_Q = 0;
    } else if (j == 1 && k == 0) {
        next_Q = 1;
    } else {
        next_Q = 1 - current_Q;
    }
}

DFlipFlop::DFlipFlop(int initialQ) : SequentialComponent(initialQ) {}

void DFlipFlop::calculateNextState(const std::vector<int>& inputs) {
    validateInputs(inputs, 1);
    next_Q = inputs[0];
}

TFlipFlop::TFlipFlop(int initialQ) : SequentialComponent(initialQ) {}

void TFlipFlop::calculateNextState(const std::vector<int>& inputs) {
    validateInputs(inputs, 1);

    if (inputs[0] == 1) {
        next_Q = 1 - current_Q;
    } else {
        next_Q = current_Q;
    }
}
