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

    void printToFile();
private:
    //structures
    nodes::Node* parseFuncDecl(TokenType type, std::string_view name, const Coordinate& location);
    nodes::Node* parseVarDecl(TokenType type, std::string_view name, const Coordinate& location);
    nodes::Node* parseVarRef(std::string_view name, const Coordinate& location);

    //bodies
    std::vector<nodes::Node*> parseGlobal();
    std::vector<nodes::Node*> parseScope();

    //keywords
    nodes::Node* parseAbort(const Coordinate& location);

    //statement tree
    nodes::Node* parseStatement();
    nodes::Node* parseExpression();
    nodes::Node* parseAssign();
    nodes::Node* parseTerm();
    nodes::Node* parseFactor();
    nodes::Node* parsePrimary();

    //helper
    Token* peek(uint ahead = 0);
    Token consume();
    bool isType(TokenType type);
    Primitive tokenTypeToPrimitive(TokenType type);
};