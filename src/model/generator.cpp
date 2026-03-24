#include "generator.hpp"
#include "sstream"

Generator::Generator(std::vector<std::unique_ptr<nodes::Node>> nodes):
nodes(std::move(nodes)), indentNum(0)
{

}

std::string Generator::generate()
{
    std::stringstream assembly;

    assembly <<".text\n\n";

    assembly << generateGlobal(nodes);

    return assembly.str();
}

#pragma region structures
std::string Generator::generateFunc(nodes::FunctionDecl* func)
{
    std::stringstream assembly;

    assembly << ".global _" << func->name <<
    "\n_" << func->name << ":\n";

    indentNum++;
    assembly << generateBlock(func->body);
    indentNum--;

    if(func->name == "main")
    {
        assembly << "\n\n\tmov x0, 0\n\tret";
    }

    return assembly.str();
}
#pragma endregion

#pragma region keywords
std::string Generator::generateExit(nodes::Exit* exit)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    assembly << "mov x0, " << nodeToFunc(exit->expr.get()) << std::endl;
    assembly << indent << "mov x16, 1\n";
    assembly << indent << "svc #0";

    return assembly.str();
}

std::string Generator::generateInt(nodes::Int* intLit)
{
    std::stringstream assembly;

    assembly << intLit->value;

    return assembly.str();
}
#pragma endregion

#pragma region blocks
std::string Generator::generateGlobal(const std::vector<std::unique_ptr<nodes::Node>>& nodes)
{
    std::stringstream assembly;

    for(auto& node : nodes)
    {
        assembly << "\n" <<nodeToFunc(node.get());
    }

    return assembly.str();
}

std::string Generator::generateBlock(const std::vector<std::unique_ptr<nodes::Node>>& nodes)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    for(auto& node: nodes)
    {
        assembly << "\n" << indent << nodeToFunc(node.get());
    }

    return assembly.str();
}
#pragma endregion

#pragma region helpers
std::string Generator::nodeToFunc(nodes::Node* node)
{
    switch(node->type)
    {
        case NodeType::function:
            return generateFunc(static_cast<nodes::FunctionDecl*>(node));
            break;
        case NodeType::exit:
            return generateExit(static_cast<nodes::Exit*>(node));
            break;
        case NodeType::intLit:
            return generateInt(static_cast<nodes::Int*>(node));
            break;
    }
}
#pragma endregion