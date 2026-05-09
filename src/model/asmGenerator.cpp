#include "asmGenerator.hpp"
#include "helpers.hpp"

AsmGenerator::AsmGenerator():indentNum(0), index(0), regNum(0)
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

    Function* function = ((Function*)(functionDecl.arg1.symbol));

    std::string name = function->name;

    int localSize = 0;

    for(const auto& [_, var]: function->locals)
    {
        localSize += helpers::typeToSize(var->type);
    }

    localSize = align16(localSize);

    assembly << ".global _" << name << std::endl;
    assembly << "_" << name << ":\n";
    indentNum++;
    generatePrologue(localSize);
    
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
            case OpCode::assign:
                generateAssign();
                break;
            default:
                break;
        }

        assembly << std::endl;

        instr = *peek();
    }

    if(!exited)
    {
        generateEpilogue(localSize);
    }

    consume();
}

void AsmGenerator::generatePrologue(int localSize)
{
    std::string indent(indentNum, '\t');

    assembly << indent << "stp x29, x30, [sp, #-16]!\n";
    assembly << indent << "mov x29, sp\n";
    assembly << indent << "add sp, sp, #" << localSize << std::endl << std::endl;
}

void AsmGenerator::generateEpilogue(int localSize)
{
    std::string indent(indentNum, '\t');

    assembly << indent << "\nmov sp, x29\n";
    assembly << indent << "ldp x29, x30, [sp], #16\n";
    assembly << indent << "sub sp, sp, #" << localSize << std::endl;
    assembly << indent << "ret\n";
}
#pragma endregion

#pragma region keywords
void AsmGenerator::generateAbort()
{
    Instruction abort = consume();
    std::string indent(indentNum, '\t');

    assembly << indent << "mov w0, w" << abort.arg1.temporary << std::endl;
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
        assembly << indent << "mov " << reg << ", #" << std::get<int>(plus.arg1.immediate) << std::endl;
        arg1 = reg;
    }
    else if(plus.arg1.kind == OperandKind::symbol)
    {
        assembly << indent << "ldr w28, [sp, #" << ((Variable*)plus.arg1.symbol)->offset << "]\n";
        arg1 = "w28";
    }

    if(plus.arg2.kind == OperandKind::temporary)
    {
        arg2 = getReg(plus.arg2.temporary);
    }
    else if(plus.arg2.kind == OperandKind::immediate)
    {
        arg2 = "#" + std::to_string(std::get<int>(plus.arg2.immediate));
    }
    else if(plus.arg2.kind == OperandKind::symbol)
    {
        int num;
        if(arg1 == "w28")
        {
            num = 27;
        }
        else
        {
            num = 28;
        }
        assembly << indent << "ldr w" << num << ", [sp, #" << ((Variable*)plus.arg2.symbol)->offset << "]\n";
        arg2 = "w" + std::to_string(num);
    }

    assembly << indent << "add " << reg << ", " << arg1 << ", " << arg2 << std::endl;
}

void AsmGenerator::generateAssign()
{
    Instruction assign = consume();

    std::string indent(indentNum, '\t');

    std::stringstream value;

    switch(assign.arg1.kind)
    {
        case OperandKind::symbol:
            value << "[sp, #" << ((Variable*)assign.arg1.symbol)->offset << "]";
            break;
        case OperandKind::temporary:
            value << "w" << assign.arg1.temporary;
            break;
        case OperandKind::immediate:
            std::visit([&value](auto& num)
            {
                value << num;
            }, assign.arg1.immediate);
            break;
    }

    assembly << indent << "mov w28"<< ", " << value.str() << std::endl;
    assembly << indent << "str w28, [sp, #" << ((Variable*)assign.result.symbol)->offset << "]\n";
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

int AsmGenerator::align16(int value)
{
    return (value + 15) & ~15;
}