#include "optimizer.hpp"
#include <ranges>
#include "helpers.hpp"

Optimizer::Optimizer()
{

}

void Optimizer::optimize(std::vector<Instruction>& iRCode)
{
    bool changed = false;
    do
    {
        changed = false;
        changed |= fold(iRCode);
        changed |= propagate(iRCode);
        changed |= eliminate(iRCode);
    } while (changed);
}

bool Optimizer::fold(std::vector<Instruction>& iRCode)
{
    bool changed = false;

    for(Instruction& instr: iRCode)
    {
        if(instr.operation == OpCode::plus)
        {
            try
            {
                instr = Instruction{.operation = OpCode::assign, .result = instr.result, .arg1 = std::to_string(std::stoi(instr.arg1) + std::stoi(instr.arg2))};
                changed = true;
            }
            catch(const std::exception& e)
            {
                
            }
            
        }
    }

    return changed;
}

bool Optimizer::propagate(std::vector<Instruction>& iRCode)
{
    bool changed = false;

    for(Instruction& instr: iRCode)
    {
        if(instr.operation == OpCode::assign && helpers::isNumber(instr.arg1))
        {
            std::string temp = instr.result;

            for(Instruction& instr2: iRCode)
            {
                if(instr2.arg1 == temp)
                {
                    instr2.arg1 = instr.arg1;
                    changed = true;
                }
                
                if(instr2.arg2 == temp)
                {
                    instr2.arg2 = instr.arg1;
                    changed = true;
                }
            }
        }
    }

    return changed;
}

bool Optimizer::eliminate(std::vector<Instruction>& iRCode)
{
    bool changed = false;

    std::vector<int> deadIndexes;
    for(uint i = 0; i < iRCode.size(); i++)
    {
        if(iRCode[i].operation == OpCode::abort)
        {
            uint next = i + 1;
            while(iRCode[next].operation != OpCode::functionEnd)
            {
                deadIndexes.emplace_back(next++);
            }
            i = next;
        }
        else if(helpers::equalsOr(iRCode[i].operation, {OpCode::assign, OpCode::plus}))
        {
            changed = true;
            for(Instruction& instr: iRCode)
            {
                if(helpers::equalsOr(iRCode[i].result, {instr.arg1, instr.arg2}))
                {
                    changed = false;
                    break;
                }
            }
            if(changed)
            {
                deadIndexes.emplace_back(i);
            }
        }
    }

    if(deadIndexes.size() > 0)
    {
        changed = true;
    }

    for(int index: deadIndexes | std::views::reverse)
    {
        iRCode.erase(iRCode.begin() + index);
    }

    return changed;
}

