#include "generator.hpp"

Generator::Generator(std::vector<std::unique_ptr<nodes::Node>> nodes, std::vector<std::unique_ptr<nodes::Symbol>> symbols):
nodes(std::move(nodes)), symbols(std::move(symbols)), indentNum(0), amountAlloc(0)
{

}

std::string Generator::generate()
{
    gatherGlobalVars(nodes);

    assembly << ".data\n\n";
    for(nodes::VarDecl* node: globalVars)
    {
        if (node->value->type == NodeType::intLit)
        {
            int value = ((nodes::Int*)(node->value.get()))->value;

            assembly << std::endl << node->name << ": .word " << node->value << std::endl;
        }
        else
        {
            throw std::runtime_error("Unrecognised value");
        }
    }


    assembly << ".text\n\n";

    generateGlobal(nodes);

    return assembly.str();
}

#pragma region expressions
void Generator::generateExpression(nodes::Node* node, std::string& result, int reg)
{
    switch(node->type)
    {
        case NodeType::intLit:
            generateInt(static_cast<nodes::Int*>(node), result, reg);
            break;
        case NodeType::varRef:
            generateVarRef(static_cast<nodes::VarRef*>(node), result, reg);
            break;
        default:
            throw std::runtime_error("Unhandled expression type.");
    }
}

void Generator::generateVarRef(nodes::VarRef* var, std::string& result, int reg)
{
    if (var->symbol->type == "int")
    {
        assembly << indent() << "ldr w" << reg << ", [sp, #" << var->symbol->stackOff << "]\n";
    }

    result = "w" + std::to_string(reg);
}

void Generator::generateInt(nodes::Int* intLit, std::string& result, int reg)
{
    result = "w" + std::to_string(reg);
    assembly << indent() << "mov w" << reg << ", #" << intLit->value << std::endl;
}
#pragma endregion

#pragma region structures
void Generator::generateVarDecl(nodes::VarDecl* var)
{
    int needed = (var->offset + var->size) - amountAlloc;
    if(needed > 0)
    {
        stackAlloc(align16(needed));
    }

    switch(var->value->type)
    {
        case NodeType::intLit:
        {
            int value = static_cast<nodes::Int*>(var->value.get())->value;

            assembly << indent() << "mov w0, #" << value << std::endl;
            assembly << indent() << "str w0, [sp, #" << var->offset << "]\n";
            break;
        }
        default:
            break;
    }
}

void Generator::generateFuncDecl(nodes::FunctionDecl* func)
{
    assembly << ".global _" << func->name <<
    "\n_" << func->name << ":\n";

    indentNum++;
    generateBlock(func->body);
    indentNum--;

    if(func->name == "main")
    {
        assembly << "\n\n\tmov x0, 0\n\tret";
    }
}
#pragma endregion

#pragma region keywords
void Generator::generateAbort(nodes::Abort* abort)
{
    std::string result;
    generateExpression(abort->expr.get(), result);

    if (result != "w0" && result != "x0")
    {
        assembly << "mov w0, " << result << std::endl;
    }
    assembly << indent() << "bl _exit\n";
}
#pragma endregion

#pragma region blocks
void Generator::generateGlobal(const std::vector<std::unique_ptr<nodes::Node>>& nodes)
{
    for(auto& node : nodes)
    {
        if(node->type == NodeType::functionDecl)
        {
            assembly << "\n";
            nodeToFunc(node.get());
        }
    }
}

void Generator::generateBlock(const std::vector<std::unique_ptr<nodes::Node>>& nodes)
{
    stackFrames.emplace_back(amountAlloc);

    for(auto& node: nodes)
    {
        assembly << "\n" << indent();
        nodeToFunc(node.get());
    }

    int target = stackFrames.back();
    stackFrames.pop_back();

    int diff = amountAlloc - target;

    if(diff > 0)
    {
        stackDealloc(diff);
    }
}
#pragma endregion

#pragma region helpers
void Generator::gatherGlobalVars(const std::vector<std::unique_ptr<nodes::Node>>& nodes)
{
    for(auto& node: nodes)
    {
        if(node->type == NodeType::varDecl)
        {
            globalVars.emplace_back((nodes::VarDecl*)node.get());
        }
    }
}

void Generator::nodeToFunc(nodes::Node* node)
{
    switch(node->type)
    {
        case NodeType::varDecl:
            generateVarDecl(static_cast<nodes::VarDecl*>(node));
            break;
        case NodeType::functionDecl:
            generateFuncDecl(static_cast<nodes::FunctionDecl*>(node));
            break;
        case NodeType::abort:
            generateAbort(static_cast<nodes::Abort*>(node));
            break;
        default:
            throw std::runtime_error("Unknown node type.");
    }
}

void Generator::stackAlloc(const int& amount)
{
    assembly << indent() << "sub sp, sp, #" << amount << std::endl;

    amountAlloc += amount;
}

void Generator::stackDealloc(const int& amount)
{
    assembly << indent() << "add sp, sp, #" << amount << std::endl;

    amountAlloc -= amount;
}

std::string Generator::indent() const
{
    return std::string(indentNum, '\t');
}

int Generator::align16(int x)
{
    return (x + 15) & ~15;
}
#pragma endregion