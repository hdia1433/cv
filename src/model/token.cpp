#include "token.hpp"

Token::Token(const int& row, const int& col, const std::string& buffer, const TokenType& type, const std::optional<std::string>& value):row(row), col(col), buffer(buffer), type(type), value(value)
{
    
}

const TokenType& Token::getType() const
{
    return this->type;
}

const std::optional<std::string>& Token::getValue() const
{
    return value;
}

const int& Token::getRow() const
{
    return row;
}

const int& Token::getCol() const
{
    return col;
}

const std::string& Token::getBuffer() const
{
    return buffer;
}

void Token::printToken()
{
    std::cout << "{" << type;
    
    if(value.has_value())
    {
        std::print(": {}", value.value());
    }
    std::print("}}");
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "{" << token.type;

    if(token.value.has_value())
    {
        os << ": " << token.value.value();
    }

    return os << "}";
}

std::ostream& operator<<(std::ostream& os, TokenType type)
{
    switch(type)
    {
        case TokenType::voidType:
            return os << "TYPE(VOID)";
        case TokenType::intType:
            return os << "TYPE(INT)";
        case TokenType::intLit:
            return os << "INT";
        case TokenType::lParen:
            return os << "LEFT_PARENTHESIS";
        case TokenType::rParen:
            return os << "RIGHT_PARENTHESIS";
        case TokenType::lBrace:
            return os << "LEFT_CURLY_BRACKET";
        case TokenType::rBrace:
            return os << "RIGHT_CURLY_BRACKET";
        case TokenType::assign:
            return os << "ASSIGN_OP";
        case TokenType::semi:
            return os << "SEMICOLON";
        case TokenType::abortKey:
            return os << "EXIT";
        case TokenType::identifier:
            return os <<"IDENTIFIER";
        
    }
}