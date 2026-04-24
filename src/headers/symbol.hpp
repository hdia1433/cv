#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "unordered_map"

namespace symbols
{
    std::unordered_map<std::string, Var> varTable;
    std::unordered_map<std::string, Func> funcTable;

    struct Var
    {
        std::string name;
        TokenType varType;
        int size;

        Var(const std::string& name, TokenType varType);
    };

    struct Func
    {
        std::string name;
        TokenType returnType;
        int size;

        Func(const std::string& name, TokenType returnType, int size);
    };
}