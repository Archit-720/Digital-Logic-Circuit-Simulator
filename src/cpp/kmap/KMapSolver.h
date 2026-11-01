#ifndef KMAP_SOLVER_H
#define KMAP_SOLVER_H

#include "KMapData.h"
#include <string>
#include <vector>

struct KMapGroup {
    std::vector<int> cells;
    std::string implicant;
    std::string explanation;
};

struct KMapResult {
    std::string expression;
    std::vector<KMapGroup> groups;
    std::vector<std::string> steps;
    bool verified = false;
};

class KMapSolver {
public:
    static KMapResult solve(const KMapData& data);
};

#endif