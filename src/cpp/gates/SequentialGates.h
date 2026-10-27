#ifndef SEQUENTIAL_GATES_H
#define SEQUENTIAL_GATES_H

#include <vector>

class SequentialComponent {
public:
    virtual ~SequentialComponent() {}

    virtual void calculateNextState(const std::vector<int>& inputs) = 0;

    void clockTick();
    int getQ() const;
    int getNotQ() const;

protected:
    explicit SequentialComponent(int initialQ = 0);
    void validateInputs(const std::vector<int>& inputs, int expectedCount) const;

    int current_Q;
    int next_Q;
};

class SRFlipFlop : public SequentialComponent {
public:
    explicit SRFlipFlop(int initialQ = 0);
    void calculateNextState(const std::vector<int>& inputs) override;
};

class JKFlipFlop : public SequentialComponent {
public:
    explicit JKFlipFlop(int initialQ = 0);
    void calculateNextState(const std::vector<int>& inputs) override;
};

class DFlipFlop : public SequentialComponent {
public:
    explicit DFlipFlop(int initialQ = 0);
    void calculateNextState(const std::vector<int>& inputs) override;
};

class TFlipFlop : public SequentialComponent {
public:
    explicit TFlipFlop(int initialQ = 0);
    void calculateNextState(const std::vector<int>& inputs) override;
};

#endif
