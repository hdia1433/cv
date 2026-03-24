#pragma once

#include "pch.hpp"

enum class TokenType
{
    voidType,
    intType,
    intLit,
    lParen, rParen,
    lBrace, rBrace,
    assign,
    semi,
    exitKey,
    identifier
};
