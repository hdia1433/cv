#include "lexer.hpp"

Lexer::Lexer(const std::string& code):
    code(code), currentIndex(0), row(1), col(1)
{

}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;

    while(peek().has_value())
    {
        int beginRow = row;
        int beginCol = col;

        if(std::isalpha(peek().value()) || peek().value() == '_')
        {
            buffer.push_back(consume());
            while(peek().has_value() && (std::isalnum(peek().value()) || peek().value() == '_' || peek().value() == '@'))
            {
                buffer.push_back(consume());
            }

            if(buffer == "void")
            {
                tokens.emplace_back(beginRow, beginCol, buffer, TokenType::voidType);
            }
            else if(buffer == "int")
            {
                tokens.emplace_back(beginRow, beginCol, buffer, TokenType::intType);
            }
            else if(buffer == "abort")
            {
                tokens.emplace_back(beginRow, beginCol, buffer, TokenType::abortKey);
            }
            else
            {
                tokens.emplace_back(beginRow, beginCol, buffer, TokenType::identifier, buffer);
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
            tokens.emplace_back(beginRow, beginCol, buffer, TokenType::intLit, buffer);
            buffer.clear();
            continue;
        }
        else if(peek().value() == ';')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, ";", TokenType::semi);
            continue;
        }
        else if(peek().value() == '(')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "(", TokenType::lParen);
        }
        else if(peek().value() == ')')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, ")", TokenType::rParen);
        }
        else if(peek().value() == '{')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "{", TokenType::lBrace);
        }
        else if(peek().value() == '}')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "}", TokenType::rBrace);
        }
        else if(peek().value() == '=')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "=", TokenType::assign);
        }
        else if (peek().value() == '\n')
        {
            consume();
            col = 1;
            row++;
        }
        else if(std::isspace(peek().value()))
        {
            consume();
            continue;
        }
        else
        {
            std::cerr << "\nAn error was found at line " << row << " and column " << col << ".\nA name cannot begin with with: " << peek().value();
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
    return code[currentIndex + amount];
}

char Lexer::consume()
{
    col++;

    return code[currentIndex++];
}