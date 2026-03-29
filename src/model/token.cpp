#include "token.hpp"

Token::Token(int row, int col, const std::string& lexeme, TokenType type, const std::optional<TokenValue>& value):row(row), col(col), lexeme(lexeme), type(type), value(value)
{
    
}

TokenType Token::getType() const
{
    return this->type;
}

const std::string& Token::getLexeme() const
{

}

const std::optional<TokenValue>& Token::getValue() const
{
    return value;
}

int Token::getRow() const
{
    return row;
}

int Token::getCol() const
{
    return col;
}

std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "{" << token.type;

    if(auto vOpt = token.value)
    {
        auto v = *vOpt;
        std::visit([&os](const auto& val)
        {
            os << ": " << val;
        }, v);
    }

    return os << "}";
}

std::ostream& operator<<(std::ostream& os, TokenType type)
{
    switch(type)
    {
        case TokenType::kwVoid:
            return os << "TYPE(VOID)";
        case TokenType::kwInt:
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
        case TokenType::kwAbort:
            return os << "EXIT";
        case TokenType::identifier:
            return os << "IDENTIFIER";
        default:
            return os << "UNKOWN";
    }
}