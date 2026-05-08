#include "asmGenerator.hpp"
#include "helpers.hpp"

AsmGenerator::AsmGenerator():indentNum(0), index(0)
{

}

std::string AsmGenerator::getAssembly()
{
    return assembly.str();
}

void AsmGenerator::generate(std::vector<Instruction>& irCode)
{
    this->irCode = &irCode;

    while(auto pInstr = peek())
    {
        auto instr = *pInstr;

        switch(instr.operation)
        {
            case OpCode::functionBegin:
                generateFunctionDecl();
                break;
            default:
                break;
        }
    }
}

#pragma region structure
void AsmGenerator::generateFunctionDecl()
{
    Instruction functionDecl = consume();

    std::string name = ((Function*)(functionDecl.arg1.symbol))->name;

    assembly << ".global _" << name << std::endl;
    assembly << "_" << name << ":\n";
    indentNum++;
    generatePrologue();
    
    bool exited = false;
    Instruction instr = *peek();
    while(instr.operation != OpCode::functionEnd)
    {
        switch(instr.operation)
        {
            case OpCode::plus:
                generatePlus();
                break;
            case OpCode::abort:
                generateAbort();
                exited = true;
                break;
            default:
                break;
        }

        instr = *peek();
    }

    if(!exited)
    {
        generateEpilogue();
    }

    consume();
}

void AsmGenerator::generatePrologue()
{
    std::string indent(indentNum, '\t');

    assembly << indent << "stp x29, x30, [sp, #-16]!\n";
    assembly << indent << "mov x29, sp\n\n";
}

void AsmGenerator::generateEpilogue()
{
    std::string indent(indentNum, '\t');

    assembly << indent << "\nmov sp, x29\n";
    assembly << indent << "ldp x29, x30, [sp], #16\n";
    assembly << indent << "ret\n";
}
#pragma endregion

#pragma region keywords
void AsmGenerator::generateAbort()
{
    Instruction abort = consume();
    std::string indent(indentNum, '\t');

    assembly << indent << "mov x0, #" <<  std::get<int>(abort.arg1.immediate) << std::endl;
    assembly << indent << "bl _exit\n";
}
#pragma endregion

#pragma region Expression tree
void AsmGenerator::generatePlus()
{
    Instruction plus = consume();
    std::string indent(indentNum, '\t');

    std::string reg;
    std::string arg1;
    std::string arg2;

    if(plus.result.kind == OperandKind::temporary)
    {
        reg = getReg(plus.result.temporary);
    }

    if(plus.arg1.kind == OperandKind::temporary)
    {
        arg1 = getReg(plus.arg1.temporary);
    }
    else if(plus.arg1.kind == OperandKind::immediate)
    {
        arg1 = "#" + std::to_string(std::get<int>(plus.arg1.immediate));
    }

    if(plus.arg2.kind == OperandKind::temporary)
    {
        arg2 = getReg(plus.arg2.temporary);
    }
    else if(plus.arg2.kind == OperandKind::immediate)
    {
        arg2 = "#" + std::to_string(std::get<int>(plus.arg2.immediate));
    }

    assembly << "\nadd " << reg << ", " << arg1 << ", " << arg2;
}
#pragma endregion


Instruction* AsmGenerator::peek()
{
    if(index >= irCode->size())
    {
        return nullptr;
    }
    return &irCode->at(index);
}

Instruction AsmGenerator::consume()
{
    return irCode->at(index++);
}

std::string AsmGenerator::getReg(int temp)
{
    return "w" + std::to_string(temp);
}