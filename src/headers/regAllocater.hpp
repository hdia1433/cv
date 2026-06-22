#pragma once

#include <unordered_map>
#include "reg.hpp"
#include "instruction.hpp"

class RegAllocater
{
private:
    std::unordered_map<Operand, int> totalUses;
    std::unordered_map<Operand, int> currentUses;
    bool registers[24];
    std::unordered_map<Operand, Reg> operands;

public:
    RegAllocater();

    void allocate(std::vector<Instruction>& irCode);

private:
    void allocateReg(Operand& op);
    void useReg(Operand& op);

    void calculateUses(std::vector<Instruction>& irCode);
    void incrementUses(Operand& op);
};
