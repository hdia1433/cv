#include "pch.hpp"
#include "token.hpp"
#include "node.hpp"

class Parser
{
private:
    std::vector<Token> tokens;
    std::vector<nodes::Node*> ast;
    uint index;

public:
    Parser();
    void parse(const std::vector<Token>& tokens);

    void print();

private:
    //bodies
    std::vector<nodes::Node*> parseGlobal();
    void parseBody();

    //keywords
    void parseAbort();

    //statement tree
    void parseStatement();
    void parseExpression();
    void parsePrimary();

    //helper
    Token* peek(uint ahead = 0);
    Token consume();
};