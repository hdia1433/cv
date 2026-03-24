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
    size_t index;

    //structures
    std::unique_ptr<nodes::Node> parseFunc(std::string, std::string);

    //keywords
    std::unique_ptr<nodes::Node> parseExit();

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
    bool isStdType(TokenType);
    std::string stdTypeToStr(TokenType);
    bool isDefType(std::string);
};