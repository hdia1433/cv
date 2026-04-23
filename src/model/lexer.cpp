#include "lexer.hpp"
#include <unordered_map>

std::unordered_map<std::string_view, TokenType> keywords = {
    {"void", TokenType::kwVoid},
    {"abort", TokenType::kwAbort}
};

Lexer::Lexer():index(0), currentLoc({1, 1})
{

}

std::vector<Token> Lexer::getTokens()
{
    return tokens;
}

void Lexer::analyze(const std::string& code)
{
    this->code = std::move(code);

    while(char* chOpt = peek())
    {
        uint start = index;

        Coordinate startLoc = currentLoc;

        char ch = *chOpt;

        if(std::isalpha(ch) || ch == '_' || ch == '@')
        {
            consume();
            while((chOpt = peek()))
            {
                ch = *chOpt;
                if(std::isalnum(ch) || ch == '_')
                {
                    consume();
                    continue;
                }
                break;
            }

            std::string_view buffer(code.data() + start, index - start);

            auto kw = keywords.find(buffer);
            if(kw != keywords.end())
            {
                addToken(buffer, kw->second, startLoc);
            }
            else
            {
                addToken(buffer, TokenType::identifier, startLoc);
            }
            
        }
        else if(std::isdigit(ch))
        {
            consume();
            while((chOpt = peek()))
            {
                ch = *chOpt;
                if(std::isdigit(ch))
                {
                    consume();
                    continue;
                }
                break;
            }

            std::string_view buffer(code.data() + start, index - start);
            addToken(buffer, TokenType::ltInt, startLoc);
        }
        else if(ch == '(')
        {
            consume();
            addToken("(", TokenType::lParen, startLoc);
        }
        else if(ch == ')')
        {
            consume();
            addToken(")", TokenType::rParen, startLoc);
        }
        else if(ch == '{')
        {
            consume();
            addToken("{", TokenType::lBrace, startLoc);
        }
        else if(ch == '}')
        {
            consume();
            addToken("}", TokenType::rBrace, startLoc);
        }
        else if(ch == '+')
        {
            consume();
            addToken("+", TokenType::opPlus, startLoc);
        }
        else if(ch == ';')
        {
            consume();
            addToken(";", TokenType::semi, startLoc);
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
    std::print("{{");
    for(Token& tok: tokens)
    {
        std::print("{{");
        switch(tok.type)
        {
            case TokenType::kwVoid:
                std::print("VOID_TYPE");
                break;
            case TokenType::kwAbort:
                std::print("KW_ABORT");
                break;
            case TokenType::ltInt:
                std::print("INT_LITERAL: {}", tok.buffer);
                break;
            case TokenType::lParen:
                std::print("LEFT_PARENTHESIS");
                break;
            case TokenType::rParen:
                std::print("RIGHT_PARENTHESIS");
                break;
            case TokenType::lBrace:
                std::print("LEFT_CURLY_BRACKET");
                break;
            case TokenType::rBrace:
                std::print("RIGHT_CURLY_BRACKET");
                break;
            case TokenType::opPlus:
                std::print("PLUS_OPERATOR");
                break;
            case TokenType::semi:
                std::print("SEMICOLON");
                break;
            case TokenType::identifier:
                std::print("IDENTIFIER: {}", tok.buffer);
                break;
        }
        std::print("}}");
    }
    std::print("}}\n");
}

char* Lexer::peek()
{
    if(index >= code.size())
    {
        return nullptr;
    }
    return &code[index];
}

char Lexer::consume()
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

void Lexer::addToken(std::string_view buffer, const TokenType& type, const Coordinate& location)
{
    Token token{.buffer = buffer, .type = type, .location = location};
    tokens.emplace_back(std::move(token));
}