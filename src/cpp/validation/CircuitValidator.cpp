#include "CircuitValidator.h"

#include <cmath>
#include <map>
#include <set>
#include <utility>

namespace {
struct ComponentSpec {
    int inputs;
    int outputs;
};

bool getComponentSpec(const std::string& type, ComponentSpec& spec) {
    if (type == "INPUT") {
        spec = {0, 1};
    } else if (type == "OUTPUT") {
        spec = {1, 0};
    } else if (type == "NOT") {
        spec = {1, 1};
    } else if (type == "AND" || type == "OR" || type == "NAND" ||
               type == "NOR" || type == "XOR" || type == "XNOR") {
        spec = {2, 1};
    } else {
        return false;
    }
    return true;
}

bool hasCycleFrom(int nodeId,
                  const std::map<int, std::vector<int>>& graph,
                  std::map<int, int>& state) {
    // State 1 means currently visiting; state 2 means finished.
    state[nodeId] = 1;
    std::map<int, std::vector<int>>::const_iterator found = graph.find(nodeId);
    if (found != graph.end()) {
        for (int nextId : found->second) {
            if (state[nextId] == 1) return true;
            if (state[nextId] == 0 && hasCycleFrom(nextId, graph, state)) return true;
        }
    }
    state[nodeId] = 2;
    return false;
}
}

bool ValidationReport::isValid() const {
    return errors.empty();
}

ValidationReport CircuitValidator::validateCircuit(const CircuitDocument& circuit) const {
    ValidationReport report;
    if (circuit.version != 1) {
        report.errors.push_back("Unsupported circuit document version.");
    }

    std::map<int, const CircuitNodeData*> nodesById;
    std::map<int, ComponentSpec> specsById;
    std::map<int, int> validInputConnections;
    std::map<int, std::set<int>> connectedPins;
    std::map<int, std::vector<int>> graph;

    for (const CircuitNodeData& node : circuit.nodes) {
        if (node.id < 0) {
            report.errors.push_back("Component IDs cannot be negative.");
        }
        if (node.type.empty()) {
            report.errors.push_back("A component has an empty type.");
        }
        if (!std::isfinite(node.x) || !std::isfinite(node.y)) {
            report.errors.push_back("Component positions must be finite numbers.");
        }
        if (nodesById.find(node.id) != nodesById.end()) {
            report.errors.push_back("Component IDs must be unique.");
            continue;
        }
        nodesById[node.id] = &node;

        ComponentSpec spec;
        if (!getComponentSpec(node.type, spec)) {
            report.errors.push_back("Unsupported component type: " + node.type + ".");
        } else {
            specsById[node.id] = spec;
        }
    }

    std::set<std::pair<int, int>> usedPins;
    for (const CircuitWireData& wire : circuit.wires) {
        std::map<int, const CircuitNodeData*>::const_iterator source = nodesById.find(wire.sourceId);
        std::map<int, const CircuitNodeData*>::const_iterator destination = nodesById.find(wire.destinationId);

        if (source == nodesById.end() || destination == nodesById.end()) {
            report.errors.push_back("A wire references a component that does not exist.");
            continue;
        }
        if (wire.sourceId == wire.destinationId) {
            report.errors.push_back("A component cannot connect to itself.");
            continue;
        }

        std::map<int, ComponentSpec>::const_iterator sourceSpec = specsById.find(wire.sourceId);
        std::map<int, ComponentSpec>::const_iterator destinationSpec = specsById.find(wire.destinationId);
        if (sourceSpec == specsById.end() || destinationSpec == specsById.end()) {
            continue;
        }
        if (sourceSpec->second.outputs == 0) {
            report.errors.push_back("The source component has no output pin.");
            continue;
        }
        if (wire.destinationInput < 0 || wire.destinationInput >= destinationSpec->second.inputs) {
            report.errors.push_back("A wire refers to an input pin that does not exist.");
            continue;
        }

        const std::pair<int, int> pin(wire.destinationId, wire.destinationInput);
        if (usedPins.find(pin) != usedPins.end()) {
            report.errors.push_back("Two wires cannot connect to the same input pin.");
            continue;
        }
        usedPins.insert(pin);
        connectedPins[wire.destinationId].insert(wire.destinationInput);
        validInputConnections[wire.destinationId] += 1;
        graph[wire.sourceId].push_back(wire.destinationId);
    }

    // Warn about required pins that have not been connected yet.
    for (const auto& item : specsById) {
        const int nodeId = item.first;
        const ComponentSpec spec = item.second;
        for (int pinIndex = 0; pinIndex < spec.inputs; ++pinIndex) {
            if (connectedPins[nodeId].find(pinIndex) == connectedPins[nodeId].end()) {
                report.warnings.push_back("Component " + std::to_string(nodeId) +
                    " has an unconnected input pin " + std::to_string(pinIndex + 1) + ".");
            }
        }
    }

    if (circuit.nodes.empty()) {
        report.warnings.push_back("The circuit is empty.");
    } else {
        bool hasOutput = false;
        for (const CircuitNodeData& node : circuit.nodes) {
            if (node.type == "OUTPUT") {
                hasOutput = true;
                if (validInputConnections[node.id] == 0) {
                    report.warnings.push_back("Output LED component " + std::to_string(node.id) +
                                              " is not connected.");
                }
            }
        }
        if (!hasOutput) {
            report.warnings.push_back("The circuit has no output LED.");
        }
    }

    std::map<int, int> state;
    for (const auto& item : nodesById) state[item.first] = 0;
    bool cycleFound = false;
    for (const auto& item : nodesById) {
        if (state[item.first] == 0 && hasCycleFrom(item.first, graph, state)) {
            cycleFound = true;
            break;
        }
    }
    if (cycleFound) {
        report.errors.push_back("A combinational cycle was detected. Check the wire connections.");
    }

    return report;
}
