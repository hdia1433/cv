#include "instruction.hpp"
#include "sstream"

bool Operand::operator==(const Operand& other) const
{
    if(kind != other.kind)
    {
        return false;
    }

    switch(kind)
    {
        case OperandKind::symbol:
            return symbol == other.symbol;
        case OperandKind::temporary:
            return temporary == other.temporary;
        case OperandKind::immediate:
            return immediate == other.immediate;
    }
}

std::string Instruction::toString()
{
    switch(operation)
    {
        case OpCode::functionBegin:
        {
            std::stringstream line;
            line << "func " << ((Function*)(arg1.symbol))->name << ":";
            return line.str();
        }
        case OpCode::functionEnd:
        {
            return "endFunc";
        }
        case OpCode::plus:
        {
            std::stringstream line;

            line << opToString(result) << " = " << opToString(arg1) << "+" << opToString(arg2);

            return line.str();
        }
        case OpCode::assign:
        {
            return opToString(result) + " = " + opToString(arg1);
        }
        case OpCode::abort:
        {
            std::stringstream line;
            line << "exit " << opToString(arg1);
            return line.str();
        }
    }
}

std::string Instruction::opToString(const Operand& op)
{
    std::stringstream line;

    switch(op.kind)
    {
        case OperandKind::symbol:
            if(op.symbol->kind == SymbolType::var)
            {
                line << ((Variable*)(op.symbol))->name;
                break;
            }
            line << ((Function*)(op.symbol))->name;
            break;
        case OperandKind::temporary:
            line << "t" << op.temporary;
            break;
        case OperandKind::immediate:
            std::visit([&line](auto& value)
            {
                line << value;
            }, op.immediate);
            break;
    }

    return line.str();
}