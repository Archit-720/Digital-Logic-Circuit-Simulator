#include "BasicGates.h"

#include <stdexcept>

AndGate::AndGate(int numberOfInputs) : Gate("AND", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("AND gate needs at least two inputs.");
    }
}

int AndGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    for (int value : inputs) {
        if (value == 0) {
            return 0;
        }
    }
    return 1;
}

OrGate::OrGate(int numberOfInputs) : Gate("OR", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("OR gate needs at least two inputs.");
    }
}

int OrGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    for (int value : inputs) {
        if (value == 1) {
            return 1;
        }
    }
    return 0;
}

NotGate::NotGate() : Gate("NOT", 1) {}

int NotGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    if (inputs[0] == 0) {
        return 1;
    }
    return 0;
}

NandGate::NandGate(int numberOfInputs) : Gate("NAND", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("NAND gate needs at least two inputs.");
    }
}

int NandGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    for (int value : inputs) {
        if (value == 0) {
            return 1;
        }
    }
    return 0;
}

NorGate::NorGate(int numberOfInputs) : Gate("NOR", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("NOR gate needs at least two inputs.");
    }
}

int NorGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    for (int value : inputs) {
        if (value == 1) {
            return 0;
        }
    }
    return 1;
}

XorGate::XorGate(int numberOfInputs) : Gate("XOR", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("XOR gate needs at least two inputs.");
    }
}

int XorGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    int result = 0;
    for (int value : inputs) {
        result = result ^ value;
    }
    return result;
}

XnorGate::XnorGate(int numberOfInputs) : Gate("XNOR", numberOfInputs) {
    if (numberOfInputs < 2) {
        throw std::invalid_argument("XNOR gate needs at least two inputs.");
    }
}

int XnorGate::evaluate(const std::vector<int>& inputs) const {
    validateInputs(inputs);

    int result = 0;
    for (int value : inputs) {
        result = result ^ value;
    }

    if (result == 0) {
        return 1;
    }
    return 0;
}
