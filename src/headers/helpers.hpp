#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "typeKind.hpp"
#include "type.hpp"

struct Type;

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

    int typeToSize(const Type& type);
}