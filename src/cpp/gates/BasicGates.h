#ifndef BASIC_GATES_H
#define BASIC_GATES_H

#include "Gate.h"

class AndGate : public Gate {
public:
    explicit AndGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

class OrGate : public Gate {
public:
    explicit OrGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

class NotGate : public Gate {
public:
    NotGate();
    int evaluate(const std::vector<int>& inputs) const override;
};

class NandGate : public Gate {
public:
    explicit NandGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

class NorGate : public Gate {
public:
    explicit NorGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

class XorGate : public Gate {
public:
    explicit XorGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

class XnorGate : public Gate {
public:
    explicit XnorGate(int numberOfInputs = 2);
    int evaluate(const std::vector<int>& inputs) const override;
};

#endif
