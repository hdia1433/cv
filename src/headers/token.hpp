#pragma once
#include "pch.hpp"

enum class TokenType
{
    kwVoid,
    kwInt,
    kwChar,
    kwAbort,
    ltInt,
    ltChar,
    lParen,
    rParen,
    lBrace,
    rBrace,
    opAssign,
    opPlus,
    opMinus,
    opMult,
    semi,
    identifier
};

enum class Primitive
{
    voidTp,
    intTp,
    charTp,
    pFunc,
    error
};

std::ostream& operator<<(std::ostream& ostream, Primitive primitive);

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