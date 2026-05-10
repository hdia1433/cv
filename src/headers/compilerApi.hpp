#pragma once
#include "pch.hpp"

struct Error
{
    std::string message;
    int line;
    int startChar;
    int endChar;
};

std::vector<Error> analyse(const std::string& source);