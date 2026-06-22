#pragma once
#include <iostream>// IWYU pragma: keep

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
    lBracket,
    rBracket,
    lBrace,
    rBrace,
    opAssign,
    opPlus,
    opMinus,
    opStar,
    opRef,
    comma,
    semi,
    identifier
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
