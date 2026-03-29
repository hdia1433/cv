#include "lexer.hpp"


static const std::unordered_map<std::string, TokenType> keywords = {
        {"void", TokenType::kwVoid},
        {"int", TokenType::kwInt},
        {"abort", TokenType::kwAbort}
    };

Lexer::Lexer(const std::string& code):
    code(code), currentIndex(0), row(1), col(1)
{

}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;

    while(std::optional<char> chOpt = peek())
    {
        char ch = *chOpt;

        int beginRow = row;
        int beginCol = col;

        if(std::isalpha(static_cast<unsigned char>(ch)) || ch == '_' || ch == '@')
        {
            buffer.push_back(consume());


            while(std::optional<char> nextOpt = peek())
            {
                char next = *nextOpt;

                if(std::isalnum((unsigned char)next) || next == '_')
                {
                    buffer.push_back(consume());
                    continue;
                }
                break;
            }

            auto word = keywords.find(buffer);
            if(word != keywords.end())
            {
                tokens.emplace_back(beginRow, beginCol, buffer, word->second);
            }
            else
            {
                tokens.emplace_back(beginRow, beginCol, buffer, TokenType::identifier);
            }
            
            buffer.clear();
        }
        else if(std::isdigit(static_cast<unsigned char>(ch)))
        {
            buffer.push_back(consume());

            while (std::optional<char> nextOpt = peek())
            {
                char next = *nextOpt;

                if(std::isdigit((unsigned char)next))
                {
                    buffer.push_back(consume());
                    continue;
                }
                break;
            }
            tokens.emplace_back(beginRow, beginCol, buffer, TokenType::intLit, std::stoi(buffer));
            buffer.clear();
            continue;
        }
        else if(ch == ';')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, ";", TokenType::semi);
            continue;
        }
        else if(ch == '(')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "(", TokenType::lParen);
        }
        else if(ch == ')')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, ")", TokenType::rParen);
        }
        else if(ch == '{')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "{", TokenType::lBrace);
        }
        else if(ch == '}')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "}", TokenType::rBrace);
        }
        else if(ch == '=')
        {
            consume();
            tokens.emplace_back(beginRow, beginCol, "=", TokenType::assign);
        }
        else if(std::isspace((unsigned char)ch))
        {
            consume();
            continue;
        }
        else
        {
            throw std::runtime_error(
                "An error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nYou cannot begin a variable name with '" + std::string(1, ch) + "'"
            );
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
    char ch = code[currentIndex++];

    if(ch == '\n')
    {
        row++;
        col = 1;
    }
    else
    {
        col++;
    }

    return ch;
}