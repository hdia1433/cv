#include "type.hpp"
#include "helpers.hpp"

std::ostream& operator<<(std::ostream& ostream, Primitive& primitive)
{
    std::visit([&ostream](const auto& value){
        using T = std::decay_t<decltype(value)>;

        bool isChar = std::is_same_v<T, char>;

        if(isChar)
        {
            ostream << "'";
        }
        ostream << value;
        if(isChar)
        {
            ostream << "'";
        }
    },primitive);

    return ostream;
}

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
    
    return true;
}

Type Type::clone() const
{
    Type t{.kind = kind};
    if(helpers::equalsOr(kind, {TypeKind::tpArray, TypeKind::tpPoint}))
    {
        t.baseType = std::make_unique<Type>(baseType->clone());
    }

    if(TypeKind::tpArray == kind)
    {
        t.size = size;
    }

    return t;
}

std::ostream& operator<<(std::ostream& ostream, Type& type)
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
            return ostream << *type.baseType << "[" << type.size << "]";
        case TypeKind::tpPoint:
            return ostream << *type.baseType << "*";
        case TypeKind::tpError:
            return ostream << "error";
    }
}