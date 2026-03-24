#pragma once
#include "pch.hpp"

enum class NodeType
{
    functionDecl,
    varDecl,
    varRef,
    symbol,
    exit,
    intLit,
    empty
};