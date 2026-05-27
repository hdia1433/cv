#include "type.hpp"
#include "helpers.hpp"
#include "typeKind.hpp"
#include <memory>

std::ostream& operator<<(std::ostream& ostream, Primitive& primitive)
{
    std::visit(
        [&ostream](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;

            bool isChar = std::is_same_v<T, char>;

            if (isChar)
            {
                ostream << "'";
            }
            ostream << value;
            if (isChar)
            {
                ostream << "'";
            }
        },
        primitive);

    return ostream;
}

Type::Type(): kind(TypeKind::tpError), baseType(nullptr), size(0)
{
}

Type::Type(TypeKind kind, Type* baseType, int size): kind(kind), baseType(baseType), size(size)
{
}

Type::Type(TypeKind kind, std::unique_ptr<Type>&& baseType, int size):
    kind(kind),
    baseType(std::move(baseType)),
    size(size)
{
}

Type::Type(const Type& other): kind(other.kind), size(other.size)
{
    if (helpers::equalsOr(kind, {TypeKind::tpArray, TypeKind::tpPoint}))
    {
        baseType = std::make_unique<Type>(*other.baseType);
    }
}

Type& Type::operator=(const Type& other)
{
    kind = other.kind;
    size = other.size;

    if (helpers::equalsOr(kind, {TypeKind::tpArray, TypeKind::tpPoint}))
    {
        baseType = std::make_unique<Type>(*other.baseType);
    }

    return *this;
}

bool Type::operator==(const Type& other) const
{
    if (kind != other.kind)
    {
        return false;
    }

    if (helpers::equalsOr(kind, {TypeKind::tpArray}))
    {
        return *baseType == *other.baseType && size == other.size;
    }

    return true;
}

std::ostream& operator<<(std::ostream& ostream, Type& type)
{
    switch (type.kind)
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
