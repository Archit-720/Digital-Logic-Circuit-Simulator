#include "KMapData.h"
#include <algorithm>
#include <stdexcept>

KMapData::KMapData(int variableCount, std::vector<int> minterms, std::vector<int> dontCares)
    : variables_(variableCount), minterms_(std::move(minterms)), dontCares_(std::move(dontCares)) {
    if (variables_ < 1 || variables_ > 8)
        throw std::invalid_argument("K-map supports 1 to 8 variables.");
    int limit = 1 << variables_;
    auto check = [limit](const std::vector<int>& values) {
        for (int x : values) if (x < 0 || x >= limit)
            throw std::invalid_argument("Minterm or don't-care is outside the variable range.");
    };
    check(minterms_);
    check(dontCares_);
    std::sort(minterms_.begin(), minterms_.end());
    minterms_.erase(std::unique(minterms_.begin(), minterms_.end()), minterms_.end());
    std::sort(dontCares_.begin(), dontCares_.end());
    dontCares_.erase(std::unique(dontCares_.begin(), dontCares_.end()), dontCares_.end());
    for (int x : minterms_)
        if (std::binary_search(dontCares_.begin(), dontCares_.end(), x))
            throw std::invalid_argument("An index cannot be both a minterm and a don't-care.");
}
int KMapData::variableCount() const { return variables_; }
const std::vector<int>& KMapData::minterms() const { return minterms_; }
const std::vector<int>& KMapData::dontCares() const { return dontCares_; }
bool KMapData::isMinterm(int index) const {
    return std::binary_search(minterms_.begin(), minterms_.end(), index);
}
bool KMapData::isDontCare(int index) const {
    return std::binary_search(dontCares_.begin(), dontCares_.end(), index);
}
int KMapData::cellValue(int index) const {
    if (isMinterm(index)) return 1;
    if (isDontCare(index)) return -1;
    return 0;
}
