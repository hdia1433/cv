#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "unordered_map"

struct Symbol
{
    std::string name;
    TokenType type;
    int size;
    int offset;
};