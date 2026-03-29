#pragma once
#include "pch.hpp"
#include "nodes.hpp"
#include <sstream>

struct Value
{
    std::string reg;
};

class Generator
{
private:
    std::stringstream assembly;
    const std::vector<std::unique_ptr<nodes::Node>> nodes;
    const std::vector<std::unique_ptr<nodes::Symbol>> symbols;
    std::vector<nodes::VarDecl*> globalVars;
    int indentNum;
    int amountAlloc;
    std::vector<int> stackFrames;

public:
    Generator(std::vector<std::unique_ptr<nodes::Node>> nodes, std::vector<std::unique_ptr<nodes::Symbol>> symbols);

    std::string generate();

private:
    //expressions
    void generateExpression(nodes::Node*, std::string& result, int reg = 0);
    void generateVarRef(nodes::VarRef*, std::string& result, int reg = 0);
    void generateInt(nodes::Int*, std::string& result, int reg = 0);


    //structures
    void generateVarDecl(nodes::VarDecl*);
    void generateFuncDecl(nodes::FunctionDecl*);

    //keywords
    void generateAbort(nodes::Abort*);

    //blocks
    void generateGlobal(const std::vector<std::unique_ptr<nodes::Node>>&);
    void generateBlock(const std::vector<std::unique_ptr<nodes::Node>>&);

    //helpers
    void gatherGlobalVars(const std::vector<std::unique_ptr<nodes::Node>>& nodes);
    void nodeToFunc(nodes::Node*);
    void stackAlloc(const int& amount = 16);
    void stackDealloc(const int& amount = 16);
    std::string indent() const;
    int align16(int x);
};