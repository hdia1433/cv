#include "instruction.hpp"
#include "sstream"

std::string Instruction::toString()
{
    switch(operation)
    {
        case OpCode::functionBegin:
        {
            std::stringstream line;
            line << "func " << arg1 << ":";
            return line.str();
        }
        case OpCode::functionEnd:
        {
            return "endFunc";
        }
        case OpCode::plus:
        {
            return result + " = " + arg1 + " + " + arg2;
        }
        case OpCode::assign:
        {
            return result + " = " + arg1;
        }
        case OpCode::abort:
        {
            std::stringstream line;
            line << "exit " << arg1;
            return line.str();
        }
    }
}