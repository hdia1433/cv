#pragma once
#include "pch.hpp"

enum class OpCode
{
    functionBegin,
    functionEnd,
    plus,
    assign,
    abort
};

struct Instruction
{
    OpCode operation;
    std::string result;
    std::string arg1;
    std::string arg2;

    std::string toString();
};