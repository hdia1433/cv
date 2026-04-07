#include "lexer.hpp"
#include <unordered_map>

std::unordered_map<std::string, TokenType> keywords = {
    {"void", TokenType::kwVoid},
    {"abort", TokenType::kwAbort}
};

Lexer::Lexer():currentLoc({1, 1})
{

}

std::vector<Token>& Lexer::getTokens()
{
    return tokens;
}

void Lexer::analyze(const std::string& code)
{
    this->code = std::move(code);

    while(char* chOpt = peek())
    {
        char ch = *chOpt;

        if(std::isalpha(ch) || ch == '_' || ch == '@')
        {
            std::string buffer = "" + consume();
            while(chOpt = peek())
            {
                ch = *chOpt;
                if(std::isalnum(ch) || ch == '_')
                {
                    buffer += consume();
                    continue;
                }
                break;
            }

            auto kw = keywords.find(buffer);
            if(kw != keywords.end())
            {
                addToken(std::move(buffer), kw->second);
            }
            else
            {
                addToken(std::move(buffer), TokenType::identifier);
            }
            
        }
        else if(std::isdigit(ch))
        {
            std::string buffer = "" + consume();
            while(chOpt = peek())
            {
                ch = *chOpt;
                if(std::isdigit(ch))
                {
                    buffer += consume();
                    continue;
                }
                break;
            }

            addToken(std::move(buffer), TokenType::ltInt);
        }
        else if(ch == '(')
        {
            consume();
            addToken("(", TokenType::lParen);
        }
        else if(ch == ')')
        {
            consume();
            addToken(")", TokenType::rParen);
        }
        else if(ch == '{')
        {
            consume();
            addToken("{", TokenType::lBrace);
        }
        else if(ch == '}')
        {
            consume();
            addToken("}", TokenType::rBrace);
        }
        else if(ch == ';')
        {
            consume();
            addToken(";", TokenType::semi);
        }
        else if(std::isspace(ch))
        {
            consume();
        }
        else
        {
            throw std::runtime_error("An error has occurred. The character '" + std::to_string(ch) + "' cannot be used to begin an identifier");
        }
    }
}

void Lexer::print()
{

}

char* Lexer::peek()
{
    return &code[index];
}

const char& Lexer::consume()
{
    char ch = code[index++];

    if(ch == '\n')
    {
        currentLoc.row++;
        currentLoc.col = 1;
    }
    else
    {
        currentLoc.col++;
    }

    return ch;
}

void Lexer::addToken(const std::string& buffer, const TokenType& type)
{
    Token token{.buffer = std::move(buffer), .type = type, .location = currentLoc};
    tokens.emplace_back(std::move(token));
}