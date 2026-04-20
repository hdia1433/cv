#include "optimizer.hpp"
#include <ranges>

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



    return changed;
}

bool Optimizer::propagate(std::vector<Instruction>& iRCode)
{
    bool changed = false;



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

