#pragma once
#include "pch.hpp"

enum class TokenType
{
    kwVoid,
    kwInt,
    kwAbort,
    ltInt,
    lParen,
    rParen,
    lBrace,
    rBrace,
    opAssign,
    opPlus,
    opMinus,
    semi,
    identifier
};

enum class Primitive
{
    voidTp,
    intTp,
    pFunc,
    custom
};

struct Coordinate
{
    uint col;
    uint row;
};

struct Token
{
    std::string_view buffer;
    TokenType type;
    Coordinate location;
};