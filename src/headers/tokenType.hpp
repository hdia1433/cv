#pragma once

#include "pch.hpp"

enum class TokenType
{
    kwVoid,
    kwInt,
    intLit,
    lParen, rParen,
    lBrace, rBrace,
    assign,
    semi,
    kwAbort,
    identifier
};
