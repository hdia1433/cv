#include "generator.hpp"
#include "sstream"

Generator::Generator(std::vector<std::unique_ptr<nodes::Node>> nodes, std::vector<std::unique_ptr<nodes::Symbol>> symbols):
nodes(std::move(nodes)), symbols(std::move(symbols)), indentNum(0), amountAlloc(0)
{

}

std::string Generator::generate()
{
    std::stringstream assembly;

    assembly <<".text\n\n";

    assembly << generateGlobal(nodes);

    return assembly.str();
}

#pragma region expressions
std::string Generator::generateExpression(nodes::Node* node, std::string& result)
{
    std::stringstream assembly;

    switch(node->type)
    {
        case NodeType::intLit:
            return generateInt(static_cast<nodes::Int*>(node), result);
        case NodeType::varRef:
            return generateVarRef(static_cast<nodes::VarRef*>(node), result);
        default:
            break;
    }

    return assembly.str();
}

std::string Generator::generateVarRef(nodes::VarRef* var, std::string& result, int reg)
{
    std::stringstream assembly;

    if (var->symbol->type == "int")
    {
        assembly << "ldr w" << reg << ", [sp, #" << var->symbol->stackOff << "]\n";
    }

    result = "w" + std::to_string(reg);

    return assembly.str();
}

std::string Generator::generateInt(nodes::Int* intLit, std::string& result)
{
    std::stringstream assembly;

    result = intLit->value;

    return assembly.str();
}
#pragma endregion

#pragma region structures
std::string Generator::generateVarDecl(nodes::VarDecl* var)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    if(var->offset + var->size > amountAlloc)
    {
        assembly << stackAlloc();
    }

    switch(var->value->type)
    {
        case NodeType::intLit:
        {
            int value = static_cast<nodes::Int*>(var->value.get())->value;

            assembly << indent << "mov w0, #" << value << std::endl;
            assembly << indent << "str w0, [sp, #" << var->offset << "]\n";
            break;
        }
        default:
            break;
    }

    

    return assembly.str();
}

std::string Generator::generateFuncDecl(nodes::FunctionDecl* func)
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
std::string Generator::generateAbort(nodes::Abort* abort)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    std::string result;
    assembly << generateExpression(abort->expr.get(), result);

    if (result != "w0")
    {
        assembly << "mov x0, " << result << std::endl;
    }
    assembly << indent << "bl _exit\n";

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

    int startingMemory = amountAlloc;

    for(auto& node: nodes)
    {
        assembly << "\n" << indent << nodeToFunc(node.get());
    }

   assembly << stackDealloc(amountAlloc - startingMemory);

    return assembly.str();
}
#pragma endregion

#pragma region helpers
std::string Generator::nodeToFunc(nodes::Node* node)
{
    switch(node->type)
    {
        case NodeType::varDecl:
            return generateVarDecl(static_cast<nodes::VarDecl*>(node));
        case NodeType::functionDecl:
            return generateFuncDecl(static_cast<nodes::FunctionDecl*>(node));
        case NodeType::abort:
            return generateAbort(static_cast<nodes::Abort*>(node));
        default:
            return "";
    }
}

std::string Generator::stackAlloc(const int& amount)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    assembly << std::endl << indent << "sub sp, sp, #" << amount << std::endl;

    amountAlloc += amount;

    return assembly.str();
}

std::string Generator::stackDealloc(const int& amount)
{
    std::stringstream assembly;

    std::string indent("\t", indentNum);

    assembly << indent << "add sp, sp, #" << amount << std::endl;

    amountAlloc -= amount;

    return assembly.str();
}
#pragma endregion