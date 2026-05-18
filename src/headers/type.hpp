#pragma once
#include "pch.hpp"
#include "helpers.hpp"
#include <concepts>
#include <type_traits>
#include <variant>

template <typename T>
concept PrimitiveName = std::is_same_v<T, int> || std::is_same_v<T, char>;

using Primitive = std::variant<int, char>;

std::ostream& operator<<(std::ostream& ostream, Primitive& primitive);

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

std::ostream& operator<<(std::ostream& ostream, Type& type);

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