#ifndef DIGITAL_LOGIC_GATE_H
#define DIGITAL_LOGIC_GATE_H

#include <vector>
#include <stdexcept>
#include <string>
#include <utility>
#include <cstddef>

// Base class for every logic gate in the simulator.
// Logic levels are represented as-> 0 = LOW, 1 = HIGH.
class Gate {
public:
    virtual ~Gate() = default;

    virtual int evaluate(const std::vector<int>& inputs) const = 0;

    std::size_t inputCount() const noexcept {
        return inputC;
    }

    const std::string& name() const noexcept {
        return n;
    }

protected:
    Gate(std::string name, std::size_t inputCount)
        : n(std::move(name)), inputC(inputCount) {
        if (inputC == 0) {
            throw std::invalid_argument("Atleast one input is required in a logic gate.");
        }
    }

    void validateInputs(const std::vector<int>& inp) const {
        if (inp.size() != inputC) {
            throw std::invalid_argument(
                n + " gate expects " + std::to_string(inputC) +
                " input(s), but received " + std::to_string(inp.size()) + ".");
        }

        for (std::size_t i = 0; i < inp.size(); ++i) {
            if (inp[i] != 0 && inp[i] != 1) {
                throw std::invalid_argument(
                    n + " gate input " + std::to_string(i + 1) +
                    " must be 0 or 1.");
            }
        }
    }

private:
    std::string n;
    std::size_t inputC;
};

#endif 
