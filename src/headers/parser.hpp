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

    std::vector<std::unique_ptr<nodes::Symbol>> getAllSymbols();

private:
    const std::vector<Token> tokens;
    std::unordered_set<std::string> types;
    unsigned int index;
    unsigned int stackOff;
    std::vector<nodes::Scope> scopeStack;
    std::vector<std::unique_ptr<nodes::Symbol>> allSymbols;
    int row;
    int col;
    bool inMainFunc;
    

    //structures
    std::unique_ptr<nodes::Node> parseVar(const std::string& type, const std::string& name);
    std::unique_ptr<nodes::Node> parseFunc(const std::string& type, const std::string& name);

    //keywords
    std::unique_ptr<nodes::Node> parseAbort();

    //statement tree
    std::unique_ptr<nodes::Node> parseStatement();
    std::unique_ptr<nodes::Node> parseExpr(const std::string& desiredType = "");
    std::unique_ptr<nodes::Node> parsePrimary(const std::string& desiredType = "");

    //blocks
    std::vector<std::unique_ptr<nodes::Node>> parseGlobal();
    std::vector<std::unique_ptr<nodes::Node>> parseBlock();

    //helper
    const Token* peek(unsigned int = 0);
    const Token& consume();
    bool isStdType(const TokenType& type);
    bool isStdType(const std::string& type);
    bool isStdLit(const TokenType& type);
    std::string stdTypeToStr(TokenType);
    std::unique_ptr<nodes::Node> stdTypeToValue(const TokenType& type, const TokenValue& value = 0);
    std::unique_ptr<nodes::Node> stdTypeToValue(const std::string& type);
    bool isDefType(std::string);
    unsigned int typeToSize(const std::string& type);
    void align(const int& size);
    void updateRowCol(const Token& token);
    nodes::Symbol* findVarInScope(const Token* token);
};