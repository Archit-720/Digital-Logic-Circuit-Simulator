#ifndef CIRCUIT_VALIDATOR_H
#define CIRCUIT_VALIDATOR_H

#include "../persistence/CircuitSerializer.h"
#include <string>
#include <vector>

struct ValidationReport {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    bool isValid() const;
};

class CircuitValidator {
public:
    // Checks node data, wire connections, input pins and combinational cycles.
    ValidationReport validateCircuit(const CircuitDocument& circuit) const;
};

#endif
