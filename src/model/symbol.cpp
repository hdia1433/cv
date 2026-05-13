#include "symbol.hpp"
#include <unistd.h>

Symbol::Symbol(SymbolType kind):kind(kind)
{

}

bool FunctionSignature::operator==(const FunctionSignature& other) const
{
    return name == other.name && returnType == other.returnType;
}

Variable::Variable(const std::string& name, Primitive type, const Coordinate& location):Symbol(SymbolType::var), name(name), type(type), location(location), used(true), global(false)
{
    
}

Function::Function(const std::string& name, Primitive returnType, const Coordinate& location):Symbol(SymbolType::func), name(name), returnType(returnType), location(location)
{
    
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