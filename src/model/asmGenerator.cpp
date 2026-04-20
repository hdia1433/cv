#include "asmGenerator.hpp"

AsmGenerator::AsmGenerator():indentNum(0), index(0)
{

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

    assembly << ".global _" << functionDecl.arg1 << std::endl;
    assembly << "_" << functionDecl.arg1 << ":\n";
    indentNum++;
    generatePrologue();
    
    bool exited = false;
    Instruction instr = *peek();
    while(instr.operation != OpCode::functionEnd)
    {
        switch(instr.operation)
        {
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
        generatePrologue();
    }
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

    assembly << indent << "mov x0, #" <<  abort.arg1 << std::endl;
    assembly << indent << "bl exit\n";
}
#pragma endregion


Instruction* AsmGenerator::peek()
{
    if(index > irCode->size())
    {
        return nullptr;
    }
    return &irCode->at(index);
}

Instruction AsmGenerator::consume()
{
    return irCode->at(index++);
}