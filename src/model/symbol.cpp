#include "symbol.hpp"
#include <unistd.h>

bool FunctionSignature::operator==(const FunctionSignature& other) const
{
    return name == other.name && returnType == other.returnType;
}

Variable::Variable(const std::string& name, Primitive type, const Coordinate& location):name(name), type(type), location(location), used(true), global(false)
{
    kind = SymbolType::var;
}

Function::Function(const std::string& name, Primitive returnType, const Coordinate& location):name(name), returnType(returnType), location(location)
{
    kind = SymbolType::func;
}

Function::~Function()
{
    for(auto& [key, var]: locals)
    {
        delete var;
    }
}

GlobalScope::GlobalScope()
{
    write(2, "Hello!\n", 7);
}

GlobalScope::~GlobalScope()
{
    for(auto& [key, value]: variables)
    {
        delete value;
    }

    for(auto& [key, value]: functions)
    {
        delete value;
    }
}

GlobalScope& getGlobalScope()
{
    static GlobalScope globalScope;
    return globalScope;
}