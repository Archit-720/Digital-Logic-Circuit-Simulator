#ifndef CIRCUIT_GENERATOR_H
#define CIRCUIT_GENERATOR_H

#include "ExpressionParser.h"
#include "../circuit/CircuitGraph.h"
#include "../gates/Gate.h"

#include <memory>
#include <string>
#include <vector>

class GeneratedCircuit {
public:
    GeneratedCircuit();

    const CircuitGraph& getGraph() const;
    int getOutputNode() const;
    int getVariableNode(const std::string& variableName) const;
    const std::vector<std::string>& getVariables() const;
    const std::vector<int>& getGateNodeIds() const;
    const Gate& getGate(int nodeId) const;

private:
    friend class CircuitGenerator;

    CircuitGraph graph;
    std::vector<std::string> variableNames;
    std::vector<int> variableNodeIds;
    std::vector<int> gateNodeIds;
    std::vector<std::unique_ptr<Gate>> gateObjects;
    int outputNodeId;
};

class CircuitGenerator {
public:
    GeneratedCircuit generate(const std::string& expression) const;

private:
    ExpressionParser parser;
};

#endif