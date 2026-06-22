#include "instruction.hpp"
#include "sstream"
#include <sstream>

bool Temporary::operator==(const Temporary& other) const
{
    return value == other.value;
}

bool Operand::operator==(const Operand& other) const
{
    return kind == other.kind && value == other.value;
}

Operand Operand::operator+(const Operand& other) const
{
    return operation(Operation::Plus, other);
}

Operand Operand::operator-(const Operand& other) const
{
    return operation(Operation::Minus, other);
}

Operand Operand::operator*(const Operand& other) const
{
    return operation(Operation::Mult, other);
}

Operand Operand::operation(Operand::Operation op, const Operand& other) const
{
    Immediate im1 = std::get<Immediate>(value);
    Immediate im2 = std::get<Immediate>(other.value);

    Operand result{.kind = OperandKind::immediate};

    std::visit([&result, &op](const auto& value, const auto& otherValue)
            {
                switch (op)
                {
                    case Operation::Plus:
                        result.value = Immediate{value + otherValue};
                        break;
                    case Operation::Minus:
                        result.value = Immediate{value - otherValue};
                        break;
                    case Operation::Mult:
                        result.value = Immediate{value * otherValue};
                        break;

                }
            }, im1, im2);

    return result;
}

std::string Instruction::toString()
{
    switch (operation)
    {
        case OpCode::functionBegin:
        {
            std::stringstream line;
            line << "func " << ((Function*)std::get<Symbol*>(arg1.value))->name << ":";
            return line.str();
        }
        case OpCode::functionEnd:
        {
            return "endFunc";
        }
        case OpCode::define:
        {
            std::stringstream line;
            line << "define " << ((Variable*)std::get<Symbol*>(arg1.value))->name;
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

            line << opToString(result) << " = " << opToString(arg1) << " - " << opToString(arg2);

            return line.str();
        }
        case OpCode::mult:
        {
            std::stringstream line;

            line << opToString(result) << " = " << opToString(arg1) << " * " << opToString(arg2);

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

    switch (op.kind)
    {
        case OperandKind::symbol:
        {
            Symbol* symbol = std::get<Symbol*>(op.value);
            if (symbol->kind == SymbolType::var)
            {
                line << ((Variable*)(symbol))->name;
                break;
            }
            line << ((Function*)(symbol))->name;
            break;
        }
        case OperandKind::reference:
            line << "&" << ((Variable*)std::get<Symbol*>(op.value))->name;
            break;
        case OperandKind::deReference:
            line << "*" << ((Variable*)std::get<Symbol*>(op.value))->name;
            break;
        case OperandKind::temporary:
            line << "t" << std::get<Temporary>(op.value).value;
            break;
        case OperandKind::tempAddress:
            line << "*t" << std::get<Temporary>(op.value).value;
            break;
        case OperandKind::immediate:
            std::visit([&line](auto& value) { line << value; }, std::get<Immediate>(op.value));
            break;
    }


    return line.str();
}
