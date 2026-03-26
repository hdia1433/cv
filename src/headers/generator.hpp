#pragma once
#include "pch.hpp"
#include "nodes.hpp"

class Generator
{
private:
    const std::vector<std::unique_ptr<nodes::Node>> nodes;
    const std::vector<std::unique_ptr<nodes::Symbol>> symbols;
    int indentNum;
    int amountAlloc;

public:
    Generator(std::vector<std::unique_ptr<nodes::Node>> nodes, std::vector<std::unique_ptr<nodes::Symbol>> symbols);

    std::string generate();

private:
    //expressions
    std::string generateExpression(nodes::Node*, std::string& result);
    std::string generateVarRef(nodes::VarRef*, std::string& result, int reg = 0);
    std::string generateInt(nodes::Int*, std::string& result);


    //structures
    std::string generateVarDecl(nodes::VarDecl*);
    std::string generateFuncDecl(nodes::FunctionDecl*);

    //keywords
    std::string generateAbort(nodes::Abort*);

    //blocks
    std::string generateGlobal(const std::vector<std::unique_ptr<nodes::Node>>&);
    std::string generateBlock(const std::vector<std::unique_ptr<nodes::Node>>&);

    //helpers
    std::string nodeToFunc(nodes::Node*);
    std::string stackAlloc(const int& amount = 16);
    std::string stackDealloc(const int& amount = 16);
};