#pragma once
#include "pch.hpp"
#include "helpers.hpp"

#ifndef TYPE
#define TYPE
enum class TypeKind
{
    tpInt,
    tpChar,
    tpVoid,
    tpArray,
    tpError
};

struct Type
{
    TypeKind kind;
    Type* baseType;
    int size;

    bool operator==(const Type& other) const;
};
#endif

std::ostream& operator<<(std::ostream& ostream, Type type);

template<>
struct std::hash<Type>
{
    std::size_t operator()(const Type& sig)
    {
        std::size_t h1 = std::hash<TypeKind>{}(sig.kind);
        if(helpers::equalsOr(sig.kind, {TypeKind::tpArray}))
        {
            std::size_t h2 = std::hash<Type>{}(*sig.baseType);
            std::size_t h3 = std::hash<int>{}(sig.size);

            return h1 ^ ((h2 << 1) ^ (h3 << 2));
        }

        return h1 ^ (h1 << 1);
    }
};