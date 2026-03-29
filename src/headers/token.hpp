#pragma once

#include "tokenType.hpp"
#include "pch.hpp"
#include <optional>
#include <variant>

using TokenValue = std::variant<int>;

class Token
{
private:
    TokenType type;
    std::string lexeme;
    std::optional<TokenValue> value;
    int row;
    int col;

public:
    Token(int row, int col, const std::string& lexeme, TokenType, const std::optional<TokenValue>& = std::nullopt);

    TokenType getType() const;
    const std::string& getLexeme() const;
    const std::optional<TokenValue>& getValue() const;
    int getRow() const;
    int getCol() const;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

std::ostream& operator<<(std::ostream& os, TokenType type);