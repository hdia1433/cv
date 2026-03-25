#pragma once

#include "pch.hpp"
#include "token.hpp"

class Lexer
{
private:
    const std::string& code;
    size_t currentIndex;
    int row;
    int col;

public:
    Lexer(const std::string&);

    std::vector<Token> tokenize();

private:
    std::optional<char> peek(int = 0) const;
    char consume();
};