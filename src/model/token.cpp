#include "token.hpp"

std::ostream& operator<<(std::ostream& ostream, Primitive primitive)
{
    switch(primitive)
    {
        case Primitive::voidTp:
            return ostream << "void";
        case Primitive::intTp:
            return ostream << "int";
        case Primitive::charTp:
            return ostream << "char";
        case Primitive::pFunc:
            return ostream << "function*";
        case Primitive::error:
            return ostream << "error";
    }
}