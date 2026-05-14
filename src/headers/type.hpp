#pragma once
#include "pch.hpp"

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
};

std::ostream& operator<<(std::ostream& ostream, Type type);