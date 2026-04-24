#include "symbol.hpp"

namespace symbols
{
    Var::Var(const std::string& name, TokenType varType):name(name), varType(varType)
    {
        switch(varType)
        {
            case TokenType::kwInt:
                size = 4;
                break;
        }
    }

    Func::Func(const std::string& name, TokenType returnType, int size):name(name), returnType(returnType), size(size)
    {
        
    }
}