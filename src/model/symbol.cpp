#include "symbol.hpp"

bool FunctionSignature::operator==(const FunctionSignature& other) const
{
    return name == other.name && returnType == other.returnType;
}

Variable::Variable(const std::string& name, Primitive type, const Coordinate& location):name(name), type(type), location(location)
{
    kind = SymbolType::var;
}

Function::Function(const std::string& name, Primitive returnType, const Coordinate& location):name(name), returnType(returnType), location(location)
{
    kind = SymbolType::func;
}

GlobalScope::~GlobalScope()
{
    for(auto& [key, value]: variables)
    {
        delete value;
    }
}

GlobalScope globalScope;