#include "iRGenerator.hpp"
#include "fstream"
#include "helpers.hpp"

IRGenerator::IRGenerator():tempNum(0), offset(0)
{

}

std::vector<Instruction>& IRGenerator::getInstructions()
{
    return instructions;
}

void IRGenerator::generate(const std::vector<nodes::Node*>& ast)
{
    currentSymbolTable = &getGlobalScope().variables;

    for(nodes::Node* node: ast)
    {
        switch(node->type)
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

    if(file.is_open())
    {
        for(Instruction instr: instructions)
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

    instructions.emplace_back(Instruction{.operation = OpCode::functionBegin, .arg1 = Operand{.kind = OperandKind::symbol, .symbol = funcDecl->symbol}});

    for(nodes::Node* node: funcDecl->body)
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
    if(!varDecl->symbol->global)
    {
        varDecl->symbol->offset = offset;
        offset += helpers::typeToSize(varDecl->symbol->type);
    }
    return Operand{.kind = OperandKind::symbol, .symbol = varDecl->symbol};
}

Operand IRGenerator::generateVarRef(nodes::VarRef* varRef)
{
    return Operand{.kind = OperandKind::symbol, .symbol = varRef->symbol};
}

Operand IRGenerator::generateBinary(nodes::Binary* binary)
{
    Operand result = {};

    if(binary->op == "+")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .temporary = (int)tempNum++};

        instructions.emplace_back(Instruction{.operation = OpCode::plus, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if(binary->op == "-")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .temporary = (int)tempNum++};

        instructions.emplace_back(Instruction{.operation = OpCode::minus, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if(binary->op == "*")
    {
        Operand arg1 = generateExpression(binary->left);
        Operand arg2 = generateExpression(binary->right);
        result = Operand{.kind = OperandKind::temporary, .temporary = (int)tempNum++};

        instructions.emplace_back(Instruction{.operation = OpCode::mult, .result = result, .arg1 = arg1, .arg2 = arg2});
    }
    else if(binary->op == "=")
    {
        switch(binary->left->type)
        {
            case NodeType::varDecl:
            {
                result = generateVarDecl((nodes::VarDecl*)binary->left);
                Operand arg1 = generateExpression(binary->right);

                instructions.emplace_back(Instruction{.operation = OpCode::assign, .result = result, .arg1 = arg1});
                break;
            }
            case NodeType::varRef:
            {
                result = generateVarRef((nodes::VarRef*)binary->left);
                Operand arg1 = generateExpression(binary->right);

                instructions.emplace_back(Instruction{.operation = OpCode::assign, .result = result, .arg1 = arg1});
                break;
            }
            default:
                break;
        }
    }

    return result;
}

void IRGenerator::generateBody(nodes::Node* node)
{
    switch(node->type)
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
    switch(node->type)
    {
        case NodeType::literal:
        {
            Operand value{.kind = OperandKind::immediate};

            std::visit([&](const auto& arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if(std::is_same_v<T, int>)
                {
                    value.immediate = arg;
                }
            }, ((nodes::Literal*)node)->value);

            return value;
        }
        case NodeType::varDecl:
            return generateVarDecl((nodes::VarDecl*)node);
        case NodeType::varRef:
            return generateVarRef((nodes::VarRef*)node);
        case NodeType::binary:
            return generateBinary((nodes::Binary*)node);
        default:
            std::println("Broken!");
            return Operand{};
    }
}