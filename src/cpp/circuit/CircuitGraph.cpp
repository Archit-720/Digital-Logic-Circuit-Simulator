#include "CircuitGraph.h"

#include <stdexcept>

int CircuitGraph::addNode() {
    adjacencyList.push_back(std::vector<int>());
    return static_cast<int>(adjacencyList.size()) - 1;
}

void CircuitGraph::addWire(int sourceId, int destinationId, int destinationInput) {
    if (!isValidNode(sourceId) || !isValidNode(destinationId)) {
        throw std::out_of_range("Wire refers to a node that does not exist.");
    }

    if (destinationInput < 0) {
        throw std::invalid_argument("Destination input number cannot be negative.");
    }

    if (hasInputConnection(destinationId, destinationInput)) {
        throw std::invalid_argument("That destination input already has a wire.");
    }

    adjacencyList[sourceId].push_back(destinationId);

    Wire newWire;
    newWire.sourceId = sourceId;
    newWire.destinationId = destinationId;
    newWire.destinationInput = destinationInput;
    wires.push_back(newWire);
}

bool CircuitGraph::detectCycle() const {
    std::vector<bool> visited(adjacencyList.size(), false);
    std::vector<bool> recursionStack(adjacencyList.size(), false);

    for (int nodeId = 0; nodeId < static_cast<int>(adjacencyList.size()); nodeId++) {
        if (!visited[nodeId]) {
            if (detectCycleFrom(nodeId, visited, recursionStack)) {
                return true;
            }
        }
    }

    return false;
}

bool CircuitGraph::detectCycleFrom(int nodeId, std::vector<bool>& visited,
                                   std::vector<bool>& recursionStack) const {
    visited[nodeId] = true;
    recursionStack[nodeId] = true;

    for (int nextNode : adjacencyList[nodeId]) {
        if (!visited[nextNode]) {
            if (detectCycleFrom(nextNode, visited, recursionStack)) {
                return true;
            }
        } else if (recursionStack[nextNode]) {
            return true;
        }
    }

    recursionStack[nodeId] = false;
    return false;
}

bool CircuitGraph::hasInputConnection(int destinationId, int destinationInput) const {
    if (!isValidNode(destinationId)) {
        throw std::out_of_range("Destination node does not exist.");
    }

    if (destinationInput < 0) {
        throw std::invalid_argument("Destination input number cannot be negative.");
    }

    for (const Wire& currentWire : wires) {
        if (currentWire.destinationId == destinationId &&
            currentWire.destinationInput == destinationInput) {
            return true;
        }
    }

    return false;
}

int CircuitGraph::nodeCount() const {
    return static_cast<int>(adjacencyList.size());
}

int CircuitGraph::wireCount() const {
    return static_cast<int>(wires.size());
}

const std::vector<int>& CircuitGraph::getConnections(int nodeId) const {
    if (!isValidNode(nodeId)) {
        throw std::out_of_range("Node does not exist.");
    }

    return adjacencyList[nodeId];
}

const std::vector<Wire>& CircuitGraph::getWires() const {
    return wires;
}

bool CircuitGraph::isValidNode(int nodeId) const {
    return nodeId >= 0 && nodeId < static_cast<int>(adjacencyList.size());
}
