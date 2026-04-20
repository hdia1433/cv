#include "iRGenerator.hpp"
#include "fstream"

IRGenerator::IRGenerator()
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
    instructions.emplace_back(Instruction{.operation = OpCode::functionBegin, .arg1 = std::string(funcDecl->name)});

    for(nodes::Node* node: funcDecl->body)
    {
        generateBody(node);
    }

    instructions.emplace_back(Instruction{.operation = OpCode::functionEnd});
}

void IRGenerator::generate(nodes::Abort* abort)
{
    std::string expression = generateExpression(abort->expression);
    instructions.emplace_back(Instruction{.operation = OpCode::abort, .arg1 = expression});
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
        case NodeType::ltInt:
            return std::to_string(((nodes::IntLit*)node)->value);
        default:
            return "";
    }
}