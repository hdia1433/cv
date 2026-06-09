#include "optimizer.hpp"
#include "helpers.hpp"
#include "instruction.hpp"
#include "symbol.hpp"
#include <algorithm>
#include <unordered_set>
#include <vector>

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
        if(instr.arg1.kind != OperandKind::immediate || instr.arg2.kind != OperandKind::immediate)
        {
            continue;
        }
        else if(helpers::equalsOr(instr.operation, {OpCode::plus}))
        {
            instr = Instruction{.operation = OpCode::assign, .result = instr.result, .arg1 = instr.arg1 + instr.arg2};
            changed = true;
        }
        else if(instr.operation == OpCode::minus)
        {
            instr = Instruction{.operation = OpCode::assign, .result = instr.result, .arg1 = instr.arg1 - instr.arg2};
            changed = true;
        }
        else if(instr.operation == OpCode::mult)
        {
            instr = Instruction{.operation = OpCode::assign, .result = instr.result, .arg1 = instr.arg1 * instr.arg2};
            changed = true;
        }
    }

    return changed;
}

bool Optimizer::propagate(std::vector<Instruction>& iRCode)
{
    bool changed = false;

    for(uint i = 0; i < iRCode.size(); i++)
    {
        Instruction& instr = iRCode[i];

        if(instr.operation == OpCode::assign && instr.arg1.kind == OperandKind::immediate)
        {
            Operand assignee = instr.result;
            std::vector<Symbol*> pointers;

            for(uint j = i + 1; j < iRCode.size(); j++)
            {
                Instruction& instr2 = iRCode[j];
                
                if(helpers::equalsOr(instr2.arg1.kind, {OperandKind::symbol, OperandKind::temporary}) && instr2.arg1 == assignee)
                {
                    instr2.arg1 = instr.arg1;
                    changed = true;
                }
                else if(OperandKind::reference == instr2.arg1.kind && instr2.operation == OpCode::assign && instr2.arg1.symbol == assignee.symbol)
                {
                    if(!std::ranges::contains(pointers, instr2.result.symbol))
                    {
                        pointers.emplace_back(instr2.result.symbol);
                    }
                }
                else if(OperandKind::deReference == instr2.arg1.kind && std::ranges::contains(pointers, instr2.arg1.symbol))
                {
                    instr2.arg1 = instr.arg1;
                    changed = true;
                }
                
                if(helpers::equalsOr(instr2.arg2.kind, {OperandKind::symbol, OperandKind::temporary}) && instr2.arg2.symbol == assignee.symbol)
                {
                    instr2.arg2 = instr.arg1;
                    changed = true;
                }
                else if(OperandKind::deReference == instr2.arg2.kind && std::ranges::contains(pointers, instr2.arg2.symbol))
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

    std::unordered_set<int> liveTemp;
    std::unordered_set<Symbol*> liveVar;

    std::vector<int> deadIndexes;

    for(uint i = 0; i < iRCode.size(); i++)
    {
        Instruction& instr = iRCode[i];

        switch(instr.operation)
        {
            case OpCode::abort:
            {
                uint next = i + 1;
                while(!helpers::equalsOr(iRCode[next].operation, {OpCode::functionEnd}))
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

    for(int i = iRCode.size() - 1; i >= 0; i--)
    {
        Instruction& instr = iRCode[i];

        bool keep = false;
        if(helpers::equalsOr(instr.operation, {OpCode::functionBegin, OpCode::functionEnd, OpCode::abort}))
        {
            keep = true;
        }
        else if((instr.result.kind == OperandKind::temporary && liveTemp.find(instr.result.temporary) != liveTemp.end()) ||
                (helpers::equalsOr(instr.result.kind, {OperandKind::symbol, OperandKind::deReference, OperandKind::reference}) && liveVar.find(instr.result.symbol) != liveVar.end()))
        {
            keep = true;
        }

        if(keep)
        {
            if(instr.result.kind == OperandKind::temporary && liveTemp.find(instr.result.temporary) != liveTemp.end())
            {
                liveTemp.erase(instr.result.temporary);
            }
            else if((helpers::equalsOr(instr.result.kind, {OperandKind::symbol, OperandKind::reference, OperandKind::deReference})) && liveVar.find(instr.result.symbol) != liveVar.end())
            {
                liveVar.erase(instr.result.symbol);
            }

            if(instr.arg1.kind == OperandKind::temporary)
            {
                liveTemp.emplace(instr.arg1.temporary);
            }
            else if((helpers::equalsOr(instr.result.kind, {OperandKind::symbol, OperandKind::reference, OperandKind::deReference})))
            {
                liveVar.emplace(instr.arg1.symbol);
            }

            if(instr.arg2.kind == OperandKind::temporary)
            {
                liveTemp.emplace(instr.arg2.temporary);
            }
            else if((helpers::equalsOr(instr.result.kind, {OperandKind::symbol, OperandKind::reference, OperandKind::deReference})))
            {
                liveVar.emplace(instr.arg2.symbol);
            }
        }
        else
        {
            if(instr.result.kind == OperandKind::symbol)
            {
                ((Variable*)instr.result.symbol)->used = false;
            }
            deadIndexes.emplace_back(i);
        }
    }

    for(int index: deadIndexes)
    {
        iRCode.erase(iRCode.begin() + index);
        changed = changed || true;
    }

    return changed;
}

