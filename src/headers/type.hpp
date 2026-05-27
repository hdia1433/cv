#pragma once
#include "typeKind.hpp"
#include <concepts>
#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

template <typename T>
concept PrimitiveName = std::is_same_v<T, int> || std::is_same_v<T, char>;

using Primitive = std::variant<int, char>;

std::ostream& operator<<(std::ostream& ostream, Primitive& primitive);

struct Type
{
    TypeKind kind;
    std::unique_ptr<Type> baseType;
    int size;

    Type();
    Type(TypeKind kind, Type* baseType = nullptr, int size = 0);
    Type(TypeKind kind, std::unique_ptr<Type>&& baseType, int size = 0);
    Type(const Type& other);

    Type& operator=(const Type& other);

    bool operator==(const Type& other) const;
};

std::ostream& operator<<(std::ostream& ostream, Type& type);

template <> struct std::hash<Type>
{
    std::size_t operator()(const Type& sig)
    {
        std::size_t h1 = std::hash<TypeKind>{}(sig.kind);
        if (TypeKind::tpArray == sig.kind)
        {
            std::size_t h2 = std::hash<Type>{}(*sig.baseType);
            std::size_t h3 = std::hash<int>{}(sig.size);

            return h1 ^ ((h2 << 1) ^ (h3 << 2));
        }

        return h1 ^ (h1 << 1);
    }
};
