#ifndef KMAP_DATA_H
#define KMAP_DATA_H

#include <vector>
#include <string>

class KMapData {
public:
    KMapData(int variableCount,
             std::vector<int> minterms,
             std::vector<int> dontCares = {});

    int variableCount() const;
    const std::vector<int>& minterms() const;
    const std::vector<int>& dontCares() const;
    bool isMinterm(int index) const;
    bool isDontCare(int index) const;
    int cellValue(int index) const; // 1=minterm, -1=don't-care, 0=otherwise

private:
    int variables_;
    std::vector<int> minterms_;
    std::vector<int> dontCares_;
};

#endif
