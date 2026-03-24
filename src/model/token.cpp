#include "token.hpp"

Token::Token(TokenType type, std::optional<std::string> value):type(type), value(value)
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
        case TokenType::exitKey:
            return os << "EXIT";
        case TokenType::identifier:
            return os <<"IDENTIFIER";
        
    }
}