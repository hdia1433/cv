#include "asmGenerator.hpp"
#include "helpers.hpp"
#include <unistd.h>

AsmGenerator::AsmGenerator():indentNum(0), index(0), regNum(0), currentSection(SectionType::text)
{

}

std::string AsmGenerator::getAssembly()
{
    return assembly.str();
}

void AsmGenerator::generate(std::vector<Instruction>& irCode)
{
    this->irCode = &irCode;

    std::vector<Instruction> staticInitInstr;

    while(auto pInstr = peek())
    {
        auto instr = *pInstr;

        switch((OpCode)instr.operation)
        {
            case OpCode::functionBegin:
                generateFunctionDecl(!staticInitInstr.empty());
                break;
            case OpCode::assign:
                if(instr.arg1.kind == OperandKind::immediate)
                {
                    std::visit([this](auto& value){
                        generateGlobalVariable(value);
                    }, instr.arg1.immediate);
                    break;
                }
                generateGlobalVariable();
                staticInitInstr.emplace_back(instr);
                break;
            case OpCode::define:
                generateGlobalVariable();
                break;
            default:
                staticInitInstr.emplace_back(consume());
                break;
        }
    }

    if(!staticInitInstr.empty())
    {
        generateStaticInit(std::move(staticInitInstr));
    }
}

#pragma region structure
void AsmGenerator::generateFunctionDecl(bool init)
{
    if(currentSection != SectionType::text)
    {
        currentSection = SectionType::text;
        assembly << std::endl << "\n.section __TEXT, __text\n";
    }

    Instruction functionDecl = consume();

    Function* function = ((Function*)(functionDecl.arg1.symbol));

    std::string name = function->name;

    int localSize = 0;

    for(const auto& [_, var]: function->locals)
    {
        if(var->used)
        {
            localSize += helpers::typeToSize(var->type);
        }
    }

    localSize = align16(localSize);

    assembly << ".global _" << name << std::endl;
    assembly << "_" << name << ":\n";
    std::string indent(++indentNum, '\t');
    generatePrologue(localSize);

    if(init)
    {
        assembly << indent << "bl _static_init\n\n";
    }
    
    bool exited = false;
    Instruction instr = *peek();
    while(instr.operation != OpCode::functionEnd)
    {
        switch(instr.operation)
        {
            case OpCode::plus:
                generatePlus();
                break;
            case OpCode::minus:
                generateMinus();
                break;
            case OpCode::mult:
                generateMult();
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

    if(localSize > 0)
    {
        assembly << indent << "sub sp, sp, #" << localSize << std::endl;
    }
    
    assembly << std::endl;
}

void AsmGenerator::generateEpilogue(int localSize)
{
    std::string indent(indentNum, '\t');

    assembly << std::endl;

    if(localSize > 0)
    {
        assembly << indent << "add sp, sp, #" << localSize << std::endl;
    }
    assembly << indent << "mov sp, x29\n";
    assembly << indent << "ldp x29, x30, [sp, #16]\n";
    assembly << indent << "ret\n";
}

void AsmGenerator::generateGlobalVariable()
{
    
    if(currentSection != SectionType::bss)
    {
        currentSection = SectionType::bss;
        assembly << ".section __DATA, __bss\n";
    }

    Instruction instr = consume();
    Variable* var = (Variable*)instr.result.symbol;

    assembly << ".lcomm _" << var->name << ", ";
    switch(var->type)
    {
        case Primitive::intTp:
            assembly << "4\n\n";
            break;
        case Primitive::charTp:
            assembly << "1\n\n";
            break;
        default:
            break;
    }
}

void AsmGenerator::generateGlobalVariable(std::variant<int, char> value)
{
    if(currentSection != SectionType::data)
    {
        currentSection = SectionType::data;
        assembly << ".section __DATA, __data\n";
    }

    Instruction instr = consume();
    Variable* var = (Variable*)instr.result.symbol;

    if(var->global)
    {
        assembly << "_" << var->name << ":\n";
    }

    switch(var->type)
    {
        case Primitive::intTp:
            assembly << "\t.long ";
            assembly << std::get<int>(value) << std::endl << std::endl;
            break;
        case Primitive::charTp:
            assembly << "\t.byte ";
            assembly << "'" << std::get<char>(value) << "'\n\n";
            break;
        default:
            break;
    }
}

void AsmGenerator::generateStaticInit(std::vector<Instruction>&& instructions)
{
    if(currentSection != SectionType::text)
    {
        currentSection = SectionType::text;
        assembly << ".section __TEXT, __text\n";
    }

    assembly << "_static_init:\n";
    int oldIndentNum = indentNum;
    indentNum = 1;

    for(Instruction& instr: instructions)
    {
        switch(instr.operation)
        {
            case OpCode::assign:
                generateAssign(instr);
                break;
            case OpCode::minus:
                generateMinus(instr);
                break;
            case OpCode::plus:
                generatePlus(instr);
                break;
            case OpCode::mult:
                generateMult(instr);
            default:
                break;
        }
    }

    assembly << std::endl << "\tret\n";

    indentNum = oldIndentNum;
}
#pragma endregion

#pragma region keywords
void AsmGenerator::generateAbort()
{
    Instruction abort = consume();
    std::string indent(indentNum, '\t');

    std::stringstream value;
    if(abort.arg1.kind == OperandKind::temporary)
    {
        value << "w" << abort.arg1.temporary;
    }
    else if(abort.arg1.kind == OperandKind::immediate)
    {
        if(const int* integer = std::get_if<int>(&abort.arg1.immediate))
        {
            value << "#" << *integer;
        }
        else if(const char* character = std::get_if<char>(&abort.arg1.immediate))
        {
            value << "#'" << *character << "'";
        }
    }
    else if(abort.arg1.kind == OperandKind::symbol)
    {
        accessVar((Variable*)abort.arg1.symbol, "w0");
    }

    if(!value.str().empty())
    {
        assembly << indent << "mov w0, " << value.str() << std::endl;
    }
    assembly << indent << "bl _exit\n";
}
#pragma endregion

#pragma region Expression tree
void AsmGenerator::generatePlus()
{
    generateOperation("add");
}

void AsmGenerator::generatePlus(const Instruction& instr)
{
    generateOperation("add", instr);
}

void AsmGenerator::generateMinus()
{
    generateOperation("sub");
}

void AsmGenerator::generateMinus(const Instruction& instr)
{
    generateOperation("sub", instr);
}

void AsmGenerator::generateMult()
{
    generateOperation("mul");
}

void AsmGenerator::generateMult(const Instruction& instr)
{
    generateOperation("mul", instr);
}

void AsmGenerator::generateAssign()
{
    generateAssign(consume());
}

void AsmGenerator::generateAssign(const Instruction& instr)
{
    std::string indent(indentNum, '\t');

    std::stringstream value;

    switch(instr.arg1.kind)
    {
        case OperandKind::symbol:
        {
            accessVar((Variable*)instr.arg1.symbol);
            break;
        }
        case OperandKind::temporary:
            value << "w" << instr.arg1.temporary;
            break;
        case OperandKind::immediate:
            if(const int* integer = std::get_if<int>(&instr.arg1.immediate))
            {
                value << "#" << *integer;
            }
            else if(const char* character = std::get_if<char>(&instr.arg1.immediate))
            {
                value << "#'" << *character << "'";
            }
            break;
    }

    if(!value.str().empty())
    {
        assembly << indent << "mov w28"<< ", " << value.str() << std::endl;
    }
    setVar((Variable*)instr.result.symbol);
}
#pragma endregion


void AsmGenerator::accessVar(Variable* symbol, const std::string& reg)
{
    std::string indent(indentNum, '\t');

    if(!symbol->global)
    {
        assembly << indent << "ldr " << reg << ", [sp, #" << symbol->offset << "]\n";
        return;
    }

    std::string xReg = "x" + reg.substr(1);
    assembly << indent << "adrp " << xReg << ", _" << symbol->name << "@PAGE\n";
    assembly << indent << "add " << xReg << ", " << xReg << ", :lo12:_" << symbol->name << "@PAGEOFF\n";
    assembly << indent << "ldr " << reg << ", [" << xReg << "]\n";
}

void AsmGenerator::setVar(Variable* symbol, const std::string& useReg, const std::string& storeReg)
{
    std::string indent(indentNum, '\t');

    if(!symbol->global)
    {
        assembly << indent << "str " << useReg << ", [sp, #" << symbol->offset << "]\n";
        return;
    }

    std::string xReg = "x" + useReg.substr(1);
    assembly << indent << "adrp " << xReg << ", _" << symbol->name << "@PAGE\n";
    assembly << indent << "add " << xReg << ", " << xReg << ", :lo12:_" << symbol->name << "@PAGEOFF\n";
    assembly << indent << "str " << storeReg << ", [" << xReg << "]\n";
}

void AsmGenerator::generateOperation(const std::string& op)
{
    generateOperation(op, consume());
}

void AsmGenerator::generateOperation(const std::string& op, const Instruction& instr)
{
    std::string indent(indentNum, '\t');

    std::string reg;
    std::string arg1;
    std::string arg2;

    if(instr.result.kind == OperandKind::temporary)
    {
        reg = getReg(instr.result.temporary);
    }

    if(instr.arg1.kind == OperandKind::temporary)
    {
        arg1 = getReg(instr.arg1.temporary);
    }  
    else if(instr.arg1.kind == OperandKind::immediate)
    {
        assembly << indent << "mov " << reg << ", ";
        if(const int* integer = std::get_if<int>(&instr.arg1.immediate))
        {
            assembly << "#" << *integer << std::endl;
        }
        else if(const char* character = std::get_if<char>(&instr.arg1.immediate))
        {
            assembly << "#'" << *character << "'\n";
        }
        arg1 = reg;
    }
    else if(instr.arg1.kind == OperandKind::symbol)
    {
        accessVar((Variable*)instr.arg1.symbol);

        arg1 = "w28";
    }

    if(instr.arg2.kind == OperandKind::temporary)
    {
        arg2 = getReg(instr.arg2.temporary);
    }
    else if(instr.arg2.kind == OperandKind::immediate)
    {
        if(const int* integer = std::get_if<int>(&instr.arg2.immediate))
        {
            arg2 = "#" + std::to_string(*integer);
        }
        else if(const char* character = std::get_if<char>(&instr.arg2.immediate))
        {
            arg2 = "#'" + std::to_string(*character);
        }

        if(op == "mul")
        {
            if(arg1 == "w28")
            {
                assembly << indent << "mov w26, " << arg2 << std::endl;
                arg2 = "w26";
            }
            else
            {
                assembly << indent << "mov w28, " << arg2 << std::endl;
                arg2 = "w28";
            }
        }

        
    }
    else if(instr.arg2.kind == OperandKind::symbol)
    {
        std::string reg;
        if(arg1.substr(1) == "28")
        {
            reg = "w27";
        }
        else
        {
            reg = "w28";
        }

        accessVar((Variable*)instr.arg2.symbol, reg);
        arg2 = reg;
    }

    assembly << indent << op << " " << reg << ", " << arg1 << ", " << arg2 << std::endl;
}

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