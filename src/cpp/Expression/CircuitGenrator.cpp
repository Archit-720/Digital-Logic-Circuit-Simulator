#include "CircuitGenrator.h"

#include "../gates/BasicGates.h"

#include <stdexcept>
#include <utility>

GeneratedCircuit::GeneratedCircuit() : outputNodeId(-1) {
}

const CircuitGraph& GeneratedCircuit::getGraph() const {
    return graph;
}

int GeneratedCircuit::getOutputNode() const {
    if (outputNodeId < 0) {
        throw std::logic_error("The generated circuit has no output node.");
    }
    return outputNodeId;
}

int GeneratedCircuit::getVariableNode(const std::string& variableName) const {
    for (std::size_t i = 0; i < variableNames.size(); i++) {
        if (variableNames[i] == variableName) {
            return variableNodeIds[i];
        }
    }

    throw std::invalid_argument("Unknown variable: " + variableName);
}

const std::vector<std::string>& GeneratedCircuit::getVariables() const {
    return variableNames;
}

const std::vector<int>& GeneratedCircuit::getGateNodeIds() const {
    return gateNodeIds;
}

const Gate& GeneratedCircuit::getGate(int nodeId) const {
    for (std::size_t i = 0; i < gateNodeIds.size(); i++) {
        if (gateNodeIds[i] == nodeId) {
            return *gateObjects[i];
        }
    }

    throw std::invalid_argument("No gate is stored for this node ID.");
}

GeneratedCircuit CircuitGenerator::generate(const std::string& expression) const {
    std::vector<std::string> postfix = parser.toPostfix(expression);
    std::vector<std::string> variables = parser.getVariables(expression);
    GeneratedCircuit generated;
    std::vector<int> nodeStack;

    for (const std::string& variable : variables) {
        int nodeId = generated.graph.addNode();
        generated.variableNames.push_back(variable);
        generated.variableNodeIds.push_back(nodeId);
    }

    for (const std::string& token : postfix) {
        if (token == "NOT") {
            if (nodeStack.empty()) {
                throw std::invalid_argument("NOT operator is missing an input.");
            }

            int inputNode = nodeStack.back();
            nodeStack.pop_back();

            std::unique_ptr<Gate> gate = std::make_unique<NotGate>();
            int gateNode = generated.graph.addNode();
            generated.graph.addWire(inputNode, gateNode, 0);

            generated.gateNodeIds.push_back(gateNode);
            generated.gateObjects.push_back(std::move(gate));
            nodeStack.push_back(gateNode);
        } else if (token == "AND" || token == "OR" || token == "XOR") {
            if (nodeStack.size() < 2) {
                throw std::invalid_argument("Binary operator is missing an input.");
            }

            int rightNode = nodeStack.back();
            nodeStack.pop_back();
            int leftNode = nodeStack.back();
            nodeStack.pop_back();

            std::unique_ptr<Gate> gate;
            if (token == "AND") {
                gate = std::make_unique<AndGate>();
            } else if (token == "OR") {
                gate = std::make_unique<OrGate>();
            } else {
                gate = std::make_unique<XorGate>();
            }

            int gateNode = generated.graph.addNode();
            generated.graph.addWire(leftNode, gateNode, 0);
            generated.graph.addWire(rightNode, gateNode, 1);

            generated.gateNodeIds.push_back(gateNode);
            generated.gateObjects.push_back(std::move(gate));
            nodeStack.push_back(gateNode);
        } else {
            nodeStack.push_back(generated.getVariableNode(token));
        }
    }

    if (nodeStack.size() != 1) {
        throw std::invalid_argument("The expression could not be converted into one output.");
    }

    generated.outputNodeId = nodeStack.back();
    return generated;
}
