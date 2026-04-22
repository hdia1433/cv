#include "optimizer.hpp"
#include <ranges>
#include "helpers.hpp"
#include <unordered_set>

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

    std::unordered_set<std::string> live;

    std::vector<int> deadIndexes;

    for(uint i = 0; i < iRCode.size(); i++)
    {
        switch(iRCode[i].operation)
        {
            case OpCode::abort:
            {
                uint next = i + 1;
                while(!helpers::equalsOr(iRCode[i].operation, {OpCode::functionEnd}))
                {
                    changed = true;
                    deadIndexes.emplace_back(next++);
                }
                i = next;
                break;
            }
            default:
                break;
        }
    }

    for(uint i = iRCode.size() - 1; i >= 0; i--)
    {
        Instruction& instr = iRCode[i];
        bool keep = false;
        if(helpers::equalsOr(instr.operation, {OpCode::functionBegin, OpCode::functionEnd, OpCode::abort}))
        {
            keep = true;
        }
        else if(!instr.result.empty() && live.find(instr.result) != live.end())
        {
            keep = true;
        }

        if(keep)
        {
            if(!instr.arg1.empty())
            {
                live.emplace(instr.arg1);
            }

            if(!instr.arg2.empty())
            {
                live.emplace(instr.arg2);
            }
        }
        else
        {
            deadIndexes.emplace_back(i);
        }
    }

    for(int index: deadIndexes | std::views::reverse)
    {
        iRCode.erase(iRCode.begin() + index);
    }

    return changed;
}

