#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "nodes.hpp"
#include <unordered_map>
#include <unordered_set>

class Parser
{
public:
    Parser(std::vector<Token>);

    std::vector<std::unique_ptr<nodes::Node>> parse();

private:
    const std::vector<Token> tokens;
    std::unordered_set<std::string> types;
    unsigned int index;
    unsigned int stackOff;
    std::vector<nodes::Scope> scopeStack;
    std::vector<std::unique_ptr<nodes::Symbol>> allSymbols;
    int row;
    int col;
    

    //structures
    std::unique_ptr<nodes::Node> parseVar(Token typeToken, const std::string name);
    std::unique_ptr<nodes::Node> parseFunc(std::string type, std::string name);

    //keywords
    std::unique_ptr<nodes::Node> parseAbort();

    //statement tree
    std::unique_ptr<nodes::Node> parseStatement();
    std::unique_ptr<nodes::Node> parseExpr();
    std::unique_ptr<nodes::Node> parsePrimary();

    //blocks
    std::vector<std::unique_ptr<nodes::Node>> parseGlobal();
    std::vector<std::unique_ptr<nodes::Node>> parseBlock();

    //helper
    std::optional<Token> peek(unsigned int = 0);
    Token consume();
    bool isStdType(const TokenType& type);
    bool isStdLit(const TokenType& type);
    std::string stdTypeToStr(TokenType);
    std::unique_ptr<nodes::Node> stdTypeToValue(const TokenType& type, const std::string& value = "");
    bool isDefType(std::string);
    unsigned int typeToSize(const TokenType& type);
    void align(const int& size);
    void updateRowCol(const Token& token);
};