#include <iostream>
#include <vector>

class CircuitGraph {
private:
    int numNodes;
    std::vector<std::vector<int>> adjList; 
public:
    CircuitGraph(int nodes) {
        numNodes = nodes;
        adjList.resize(nodes);
    }
};