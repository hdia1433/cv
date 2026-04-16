#include "pch.hpp"
#include "token.hpp"
#include "node.hpp"

class Parser
{
private:
    std::vector<Token> tokens;
    std::vector<nodes::Node*> ast;
    uint index;
    std::vector<nodes::Error*> errors;

public:
    Parser();
    ~Parser();
    void parse(const std::vector<Token>& tokens);

    std::vector<nodes::Node*> getAst();

    void print();
private:
    //structures
    nodes::Node* parseFuncDecl(TokenType type, std::string_view name);


    //bodies
    std::vector<nodes::Node*> parseGlobal();
    std::vector<nodes::Node*> parseBody();

    //keywords
    nodes::Node* parseAbort();

    //statement tree
    nodes::Node* parseStatement();
    nodes::Node* parseExpression();
    nodes::Node* parsePrimary();

    //helper
    Token* peek(uint ahead = 0);
    Token consume();
    bool isType(TokenType type);
};