#include "iRGenerator.hpp"
#include "fstream"

IRGenerator::IRGenerator():tempNum(0)
{

}

std::vector<Instruction>& IRGenerator::getInstructions()
{
    return instructions;
}

void IRGenerator::generate(const std::vector<nodes::Node*>& ast)
{
    for(nodes::Node* node: ast)
    {
        switch(node->type)
        {
            case NodeType::funcDecl:
            {
                generate((nodes::FuncDecl*)node);
                break;
            }
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

    instructions.emplace_back(Instruction{.operation = OpCode::functionBegin, .arg1 = std::string(funcDecl->name)});

    for(nodes::Node* node: funcDecl->body)
    {
        generateBody(node);
    }

    instructions.emplace_back(Instruction{.operation = OpCode::functionEnd});

    tempNum = startTempNum;
}

void IRGenerator::generate(nodes::Abort* abort)
{
    instructions.emplace_back(Instruction{.operation = OpCode::abort, .arg1 = generateExpression(abort->expression)});
}

std::string IRGenerator::generateBinary(nodes::Binary* binary)
{
    std::string temp = "t" + std::to_string(tempNum++);

    if(binary->op == "+")
    {
        instructions.emplace_back(Instruction{.operation = OpCode::plus, .result = temp, .arg1 = generateExpression(binary->left), .arg2 = generateExpression(binary->right)});
    }

    return temp;
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
        default:
            break;
    }
}

std::string IRGenerator::generateExpression(nodes::Node* node)
{
    switch(node->type)
    {
        case NodeType::literal:
            return ""; //std::to_string(((nodes::Literal*)node)->value);
        case NodeType::binary:
            return generateBinary((nodes::Binary*)node);
        default:
            std::println("Broken!");
            return "";
    }
}