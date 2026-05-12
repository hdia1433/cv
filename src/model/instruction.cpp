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

Operand Operand::operator+(const Operand& other) const
{
    return operation(other, "+");
}

Operand Operand::operator-(const Operand& other) const
{
    return operation(other, "-");
}

Operand Operand::operation(const Operand& other, const std::string& op) const
{
    if(kind != OperandKind::immediate || other.kind != OperandKind::immediate)
    {
        std::cerr << "Error, cannot add 2 operands that are not both immediates";
        throw std::runtime_error("math error");
    }

    Operand result{.kind = OperandKind::immediate};

    std::visit([&other, &op, &result, this](const auto& value, const auto& otherValue)
    {
        if(op == "+")
        {
            result.immediate = value + otherValue;
        }
        else if(op == "-")
        {
            result.immediate = value - otherValue;
        }
    }, immediate, other.immediate);

    return result;
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
        case OpCode::define:
        {
            std::stringstream line;
            line << "define " << ((Variable*)arg1.symbol)->name;
            return line.str();
        }
        case OpCode::plus:
        {
            std::stringstream line;

            line << opToString(result) << " = " << opToString(arg1) << "+" << opToString(arg2);

            return line.str();
        }
        case OpCode::minus:
        {
            std::stringstream line;

            line << opToString(result) << "=" << opToString(arg1) << "-" << opToString(arg2);

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