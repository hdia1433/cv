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

public:
    Token(TokenType, std::optional<std::string> = std::nullopt);

    const TokenType& getType() const;
    const std::optional<std::string>& getValue() const;

    void printToken();

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

std::ostream& operator<<(std::ostream& os, TokenType type);