#pragma once
#include "pch.hpp"
#include "instruction.hpp"
#include <unordered_map>

class Optimizer
{
private:

public:
    Optimizer();

    void optimize(std::vector<Instruction>& iRCode);

private:
    bool fold(std::vector<Instruction>& iRCode);
    bool propagate(std::vector<Instruction>& iRCode);
    bool eliminate(std::vector<Instruction>& iRCode);
};