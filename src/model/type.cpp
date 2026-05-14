#include "type.hpp"
#include "helpers.hpp"

bool Type::operator==(const Type& other) const
{
    if(kind != other.kind)
    {
        return false;
    }

    if(helpers::equalsOr(kind, {TypeKind::tpArray}))
    {
        return *baseType == *other.baseType && size == other.size;
    }
    
    return false;
}

std::ostream& operator<<(std::ostream& ostream, Type type)
{
    switch(type.kind)
    {
        case TypeKind::tpVoid:
            return ostream << "void";
        case TypeKind::tpInt:
            return ostream << "int";
        case TypeKind::tpChar:
            return ostream << "char";
        case TypeKind::tpArray:
            return ostream << "array<" << type.baseType << ", " << type.size << ">";
        case TypeKind::tpError:
            return ostream << "error";
    }
}