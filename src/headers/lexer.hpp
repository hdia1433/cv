#include "pch.hpp"
#include "token.hpp"

class Lexer
{
private:
    u_int index;
    std::string code;
    std::vector<Token> tokens;
    Coordinate currentLoc;
public:
    Lexer();

    std::vector<Token>& getTokens();

    void analyze(const std::string& code);

    void print();
private:
    char* peek();
    char consume();
    void addToken(std::string_view buffer, const TokenType& type);
};