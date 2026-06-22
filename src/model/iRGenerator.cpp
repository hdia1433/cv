#include "iRGenerator.hpp"
#include "fstream"
#include "helpers.hpp"
#include "instruction.hpp"
#include "node.hpp"
#include "symbol.hpp"
#include <cstddef>
#include <print>
#include <stdexcept>

IRGenerator::IRGenerator(): tempNum(0), offset(0)
{
}

std::vector<Instruction>& IRGenerator::getInstructions()
{
    return instructions;
}

void IRGenerator::generate(const std::vector<nodes::Node*>& ast)
{
    currentSymbolTable = &getGlobalScope().variables;

    for (nodes::Node* node : ast)
    {
        switch (node->type)
        {
            case NodeType::funcDecl:
            {
                generate((nodes::FuncDecl*)node);
                break;
            }
            case NodeType::varDecl:
            {
                Operand var = generateVarDecl((nodes::VarDecl*)node);
                instructions.emplace_back(Instruction{.operation = OpCode::define, .arg1 = var});
                break;
            }
            case NodeType::binary:
                generateBinary((nodes::Binary*)node);
                break;
            default:
                break;
        }
    }
}

void IRGenerator::printToFile(const std::string& fileName)
{
    std::ofstream file(fileName);

    if (file.is_open())
    {
        for (Instruction instr : instructions)
        {
            file << instr.toString() << std::endl;
        }

        file.close();
    }
    else
    {
        std::println("Failed to open file.");
    }
}

void IRGenerator::generate(nodes::FuncDecl* funcDecl)
{
    uint startTempNum = tempNum;
    tempNum = 0;
    auto oldSymbolTable = currentSymbolTable;
    currentSymbolTable = &funcDecl->symbol->locals;

    instructions.emplace_back(Instruction{.operation = OpCode::functionBegin,
                                          .arg1 = Operand{.kind = OperandKind::symbol, .value = funcDecl->symbol}});

    for (nodes::Node* node : funcDecl->body)
    {
        generateBody(node);
    }

    instructions.emplace_back(Instruction{.operation = OpCode::functionEnd});

    tempNum = startTempNum;
    currentSymbolTable = oldSymbolTable;
}

void IRGenerator::generate(nodes::Abort* abort)
{
    Operand arg1 = generateExpression(abort->expression);

    instructions.emplace_back(Instruction{.operation = OpCode::abort, .arg1 = arg1});
}

Operand IRGenerator::generateVarDecl(nodes::VarDecl* varDecl)
{
    if (!varDecl->symbol->global)
    {
        varDecl->symbol->loc = offset;
        offset += helpers::typeToSize(varDecl->symbol->type);
    }
    return Operand{.kind = OperandKind::symbol, .value = varDecl->symbol};
}

Operand IRGenerator::generateVarRef(nodes::VarRef* varRef)
{
    return Operand{.kind = OperandKind::symbol, .value = varRef->symbol};
}

Operand IRGenerator::generateBinary(nodes::Binary* binary)
{
    Operand result = {};

    if (binary->op == "+")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .value = Temporary{(int)tempNum++}};

        instructions.emplace_back(Instruction{.operation = OpCode::plus, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if (binary->op == "-")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .value = Temporary{(int)tempNum++}};

        instructions.emplace_back(
            Instruction{.operation = OpCode::minus, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if (binary->op == "*")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .value = Temporary{(int)tempNum++}};

        instructions.emplace_back(Instruction{.operation = OpCode::mult, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if (binary->op == "=")
    {
        switch (binary->left->type)
        {
            case NodeType::varDecl:
                result = generateVarDecl((nodes::VarDecl*)binary->left);
                break;
            case NodeType::varRef:
                result = generateVarRef((nodes::VarRef*)binary->left);
                break;
            case NodeType::unary:
                result = generateUnary((nodes::Unary*)binary->left);
                break;
            case NodeType::subscript:
                result = generateSubscript((nodes::Subscript*)binary->left);
            default:
                break;
        }

        if(NodeType::initList == binary->right->type)
        {
            generateInitList(result, (nodes::InitList*)binary->right);
        }
        else
        {
            Operand arg1 = generateExpression(binary->right);
            instructions.emplace_back(Instruction{.operation = OpCode::assign, .result = result, .arg1 = arg1});
        }
    }

    return result;
}

Operand IRGenerator::generateUnary(nodes::Unary* unary)
{
    if ("&" == unary->op)
    {
        return Operand{.kind = OperandKind::reference,
                       .value = generateVarRef((nodes::VarRef*)unary->expression.get()).value};
    }
    else if ("*" == unary->op)
    {
        return Operand{.kind = OperandKind::deReference,
                       .value = generateVarRef((nodes::VarRef*)unary->expression.get()).value};
    }
    return generateExpression(unary->expression.get());
}

void IRGenerator::generateInitList(Operand base, nodes::InitList* initList)
{
    switch(base.kind)
    {
        case OperandKind::symbol:
            base.kind = OperandKind::reference;
            break;
        case OperandKind::deReference:
            base.kind = OperandKind::symbol;
            break;
        case OperandKind::tempAddress:
            base.kind = OperandKind::temporary;
            break;
        default:
            throw std::runtime_error("Broken in generateInitList(Operand, InitList*) in iRGenerator.cpp");
    }

    int temp = (int)tempNum++;

    for(uint i = 0; i < initList->values.size(); i++)
    {
        Operand index = {.kind = OperandKind::temporary, .value = Temporary{temp}};
        if(0 != i)
        {
            instructions.emplace_back(Instruction{.operation = OpCode::plus, .result = index, .arg1 = base, .arg2 = Operand{.kind = OperandKind::immediate, .value = (int)i * helpers::typeToSize(initList->baseType)}});
        }
        else
        {
            instructions.emplace_back(Instruction{.operation = OpCode::assign, .result = index, .arg1 = base});
        }
        index.kind = OperandKind::tempAddress;
        nodes::Node*& value = initList->values[i];
        if(value->type == NodeType::initList)
        {
            generateInitList(index, (nodes::InitList*)value);
        }
        else
        {
            instructions.emplace_back(Instruction{.operation = OpCode::assign, .result = index, .arg1 = generateExpression(value)});
        }
    }
}

Operand IRGenerator::generateSubscript(nodes::Subscript* subscript)
{
    nodes::VarRef* expression = (nodes::VarRef*)subscript->expression.get();
    Variable* symbol = (Variable*)expression->symbol;

    Operand pointer = {.kind = OperandKind::symbol, .value = symbol};

    if(TypeKind::tpArray == symbol->type.kind)
    {
        pointer.kind = OperandKind::reference;
    }

    Operand index = {.kind = OperandKind::temporary, .value = Temporary{(int)tempNum++}};
    instructions.emplace_back(Instruction{.operation = OpCode::mult, .result = index, .arg1 = generateExpression(subscript->index.get()), .arg2 = Operand{.kind = OperandKind::immediate, .value = helpers::typeToSize(*symbol->type.baseType)}});

    instructions.emplace_back(Instruction{.operation = OpCode::plus, .result = index, .arg1 = pointer, .arg2 = index});

    index.kind = OperandKind::tempAddress;

    return index;
}

void IRGenerator::generateBody(nodes::Node* node)
{
    switch (node->type)
    {
        case NodeType::kwAbort:
        {
            generate((nodes::Abort*)node);
            break;
        }
        case NodeType::binary:
            generateBinary((nodes::Binary*)node);
            break;
        default:
            break;
    }
}

Operand IRGenerator::generateExpression(nodes::Node* node)
{
    switch (node->type)
    {
        case NodeType::literal:
        {
            Operand value{.kind = OperandKind::immediate};

            nodes::Literal* lit = (nodes::Literal*)node;

            if (const int* integer = std::get_if<int>(&lit->value))
            {
                value.value = *integer;
            }
            else if (const char* character = std::get_if<char>(&lit->value))
            {
                value.value = *character;
            }

            return value;
        }
        case NodeType::varDecl:
            return generateVarDecl((nodes::VarDecl*)node);
        case NodeType::varRef:
            return generateVarRef((nodes::VarRef*)node);
        case NodeType::binary:
            return generateBinary((nodes::Binary*)node);
        case NodeType::unary:
            return generateUnary((nodes::Unary*)node);
        case NodeType::subscript:
            return generateSubscript((nodes::Subscript*)node);
        default:
            std::println("Broken in generateExpression(Node*) in iRGenerator.cpp.");
            return Operand{};
    }
}
