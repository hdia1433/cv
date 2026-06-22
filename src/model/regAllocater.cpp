#include "regAllocater.hpp"

RegAllocater::RegAllocater()
{
    std::fill(std::begin(registers), std::end(registers), false);
}

void RegAllocater::allocate(std::vector<Instruction>& irCode)
{
    calculateUses(irCode);

    for(Instruction& instr : irCode)
    {
       switch (instr.operation)
       {
            case OpCode::assign:
                allocateReg(instr.result);
                useReg(instr.arg1);
                break;
            
       }
    }
}

void RegAllocater::allocateReg(Operand& op)
{
    if (OperandKind::immediate == op.kind)
    {
        return;
    }

    if (operands.find(op) != operands.end())
    {
        return;
    }

    for (int i = 0; i < std::size(registers); i++)
    {
        if (!registers[i])
        {
            operands.emplace(op, (Reg)i);
            currentUses.emplace(op, 0);
            registers[i] = true;
            
            if (Symbol** pSymbol = std::get_if<Symbol*>(&op.value); pSymbol && SymbolType::var == (*pSymbol)->kind)
            {
                Variable* symbol = (Variable*)*pSymbol;

                symbol->loc = (Reg)i;
            }

            return;
        }
    }

    
}

void RegAllocater::useReg(Operand& op)
{
    if (OperandKind::immediate == op.kind)
    {
        return;
    }

    if (operands.find(op) == operands.end())
    {
        allocateReg(op);
    }

    if(totalUses[op] <= ++currentUses[op])
    {
        registers[(int)operands[op]] = false;
    }
}

void RegAllocater::calculateUses(std::vector<Instruction>& irCode)
{
    for(Instruction& instr: irCode)
    {
        switch(instr.operation)
        {
            case OpCode::assign:
            case OpCode::abort:
                incrementUses(instr.arg1);
                break;
            case OpCode::plus:
            case OpCode::minus:
            case OpCode::mult:
                incrementUses(instr.arg1);
                incrementUses(instr.arg2);
                break;
            default:
                std::println("Broken in RegAllocater::calculateUses(vector<Instruction>&).");
                break;
        }
    }
}

void RegAllocater::incrementUses(Operand& op)
{
    if (op.kind == OperandKind::immediate)
    {
        return;
    }

    if (totalUses.find(op) == totalUses.end())
    {
        totalUses.emplace(op, 1);
    }
    else
    {
        totalUses[op]++;
    }
}
