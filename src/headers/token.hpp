#include "pch.hpp"

enum class TokenType
{
    kwVoid,
    kwAbort,
    ltInt,
    lParen,
    rParen,
    lBrace,
    rBrace,
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
    std::string buffer;
    TokenType type;
    Coordinate location;

    inline Coordinate operator->() const
    {
        return location;
    }
};