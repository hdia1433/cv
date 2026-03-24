#include "lexer.hpp"

Lexer::Lexer(const std::string& code):
    code(code), currentIndex(0)
{

}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;

    while(peek().has_value())
    {
        if(std::isalpha(peek().value()) || peek().value() == '_')
        {
            buffer.push_back(consume());
            while(peek().has_value() && (std::isalnum(peek().value()) || peek().value() == '_'))
            {
                buffer.push_back(consume());
            }

            if(buffer == "void")
            {
                tokens.emplace_back(TokenType::voidType);
            }
            else if(buffer == "int")
            {
                tokens.emplace_back(TokenType::intType);
            }
            else if(buffer == "exit")
            {
                tokens.emplace_back(TokenType::exitKey);
            }
            else
            {
                tokens.emplace_back(TokenType::identifier, buffer);
            }
            buffer.clear();
            continue;
        }
        else if(std::isdigit(peek().value()))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
            {
                buffer.push_back(consume());
            }
            tokens.emplace_back(TokenType::intLit, buffer);
            buffer.clear();
            continue;
        }
        else if(peek().value() == ';')
        {
            consume();
            tokens.emplace_back(TokenType::semi);
            continue;
        }
        else if(peek().value() == '(')
        {
            consume();
            tokens.emplace_back(TokenType::lParen);
        }
        else if(peek().value() == ')')
        {
            consume();
            tokens.emplace_back(TokenType::rParen);
        }
        else if(peek().value() == '{')
        {
            consume();
            tokens.emplace_back(TokenType::lBrace);
        }
        else if(peek().value() == '}')
        {
            consume();
            tokens.emplace_back(TokenType::rBrace);
        }
        else if(peek().value() == '=')
        {
            consume();
            tokens.emplace_back(TokenType::assign);
        }
        else if(std::isspace(peek().value()))
        {
            consume();
            continue;
        }
        else
        {
            std::cerr << "Error! Cannot begin token with: " << peek().value();
            exit(EXIT_FAILURE);
        }
    }

    currentIndex = 0;
    return tokens;
}

std::optional<char> Lexer::peek(int amount) const
{
    if (currentIndex + amount >= code.length())
    {
        return{};
    }
    return code[currentIndex];
}

char Lexer::consume()
{
    return code[currentIndex++];
}