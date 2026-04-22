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

    assembly << indent << "mov x0, #" <<  abort.arg1 << std::endl;
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

    if(plus.result[0] == 't')
    {
        reg = getReg(plus.result);
    }

    if(plus.arg1[0] == 't')
    {
        arg1 = getReg(plus.arg1);
    }
    else if(helpers::isNumber(plus.arg1))
    {
        arg1 = "#" + plus.arg1;
    }

    if(plus.arg2[0] == 't')
    {
        arg2 = getReg(plus.arg2);
    }
    else if(helpers::isNumber(plus.arg2))
    {
        arg2 = "#" + plus.arg2;
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

std::string AsmGenerator::getReg(const std::string& temp)
{
    return "w" + temp.substr(1);
}