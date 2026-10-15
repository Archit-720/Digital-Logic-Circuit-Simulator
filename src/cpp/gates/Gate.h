#ifndef GATE_H
#define GATE_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

class Gate {
public:
    virtual ~Gate() {}

    virtual int evaluate(const std::vector<int>& inputs) const = 0;

    int inputCount() const {
        return inputCount_;
    }

    const std::string& name() const {
        return name_;
    }

protected:
    Gate(const std::string& gateName, int numberOfInputs) {
        name_ = gateName;
        inputCount_ = numberOfInputs;
    }

    void validateInputs(const std::vector<int>& inputs) const {
        if (inputs.size() != static_cast<std::size_t>(inputCount_)) {
            throw std::invalid_argument("Incorrect number of inputs for " + name_ + " gate.");
        }

        for (int value : inputs) {
            if (value != 0 && value != 1) {
                throw std::invalid_argument("Gate inputs must be 0 or 1.");
            }
        }
    }

private:
    std::string name_;
    int inputCount_;
};

#endif
