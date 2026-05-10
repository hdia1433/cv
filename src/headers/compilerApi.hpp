#pragma once
#include "pch.hpp"

struct Error
{
    std::string message;
    int line;
    int column;
};

struct AnalysisResult
{
    std::vector<Error> errors;
};

AnalysisResult analyse(const std::string& source);