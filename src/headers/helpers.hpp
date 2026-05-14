#pragma once
#include "pch.hpp"
#include "token.hpp"

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

namespace helpers
{
    template<typename T>
    bool equalsOr(T comparer, std::initializer_list<T> against)
    {
        for(T thing: against)
        {
            if(comparer == thing)
            {
                return true;
            }
        }
        return false;
    }

    bool isInt(std::string str);

    bool isFloat(std::string str);

    bool isDouble(std::string str);

    bool isNumber(std::string str);

    int typeToSize(Type type);
}