#include "lexer.hpp"
#include <unordered_map>
#include <sstream>
#include <fstream>

std::unordered_map<std::string_view, TokenType> keywords = {
    {"void", TokenType::kwVoid},
    {"int", TokenType::kwInt},
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
        else if(ch == '=')
        {
            consume();
            addToken("=", TokenType::opAssign, startLoc);
        }
        else if(ch == '+')
        {
            consume();
            addToken("+", TokenType::opPlus, startLoc);
        }
        else if(ch == '-')
        {
            consume();
            addToken("-", TokenType::opMinus, startLoc);
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
            std::cerr << "An error has occurred at the line " << currentLoc.row << " and the column " << currentLoc.col << ".\nThe character '" << ch << "' cannot be used to begin an identifier.\n\n";
            throw std::runtime_error("1 error was generated.");
        }
    }
}

void Lexer::printToFile()
{
    std::ofstream file("tokens.txt");
    if(!file)
    {
        std::println("Error loading file tokens.txt");
    }
    file << "{";
    for(Token& tok: tokens)
    {
        file << "{";
        switch(tok.type)
        {
            case TokenType::kwVoid:
                file << "VOID_TYPE";
                break;
            case TokenType::kwInt:
                file << "INTEGER_TYPE";
                break;
            case TokenType::kwAbort:
                file << "KW_ABORT";
                break;
            case TokenType::ltInt:
                file << "INT_LITERAL: " << tok.buffer;
                break;
            case TokenType::lParen:
                file << "LEFT_PARENTHESIS";
                break;
            case TokenType::rParen:
                file << "RIGHT_PARENTHESIS";
                break;
            case TokenType::lBrace:
                file << "LEFT_CURLY_BRACKET";
                break;
            case TokenType::rBrace:
                file << "RIGHT_CURLY_BRACKET";
                break;
            case TokenType::opAssign:
                file << "ASSIGNMENT_OPERATOR";
                break;
            case TokenType::opPlus:
                file << "PLUS_OPERATOR";
                break;
            case TokenType::semi:
                file << "SEMICOLON";
                break;
            case TokenType::identifier:
                file << "IDENTIFIER: " << tok.buffer;
                break;
        }
        file << "}, \n";
    }
    file << "}";
    std::println("Token list printed to tokens.txt");
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