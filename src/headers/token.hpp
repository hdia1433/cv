#pragma once

#include "tokenType.hpp"
#include "pch.hpp"
#include <optional>
#include <variant>

class Token
{
private:
    TokenType type;
    std::optional<std::string> value;
    int row;
    int col;
    std::string buffer;

public:
    Token(const int& col, const int& row, const std::string& buffer, const TokenType&, const std::optional<std::string>& = std::nullopt);

    const TokenType& getType() const;
    const std::optional<std::string>& getValue() const;
    const int& getRow() const;
    const int& getCol() const;
    const std::string& getBuffer() const;

    void printToken();

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

std::ostream& operator<<(std::ostream& os, TokenType type);