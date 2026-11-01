#ifndef CIRCUIT_GRAPH_H
#define CIRCUIT_GRAPH_H

#include <vector>

struct Wire {
    int sourceId;
    int destinationId;
    int destinationInput;
};

class CircuitGraph {
public:
    int addNode();

    void addWire(int sourceId, int destinationId, int destinationInput);

    bool detectCycle() const;
    bool hasInputConnection(int destinationId, int destinationInput) const;

    int nodeCount() const;
    int wireCount() const;

    const std::vector<int>& getConnections(int nodeId) const;
    const std::vector<Wire>& getWires() const;

private:
    std::vector<std::vector<int>> adjacencyList;
    std::vector<Wire> wires;

    bool isValidNode(int nodeId) const;
    bool detectCycleFrom(int nodeId, std::vector<bool>& visited,
                         std::vector<bool>& recursionStack) const;
};

#endif
