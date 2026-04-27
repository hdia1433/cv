#pragma once
#include "pch.hpp"
#include "node.hpp"

using ScopeStack = std::vector<std::vector<std::string>>;

class SemanticAnalyser
{
private:
    std::vector<std::string> errors;
    ScopeStack scopeStack;
    bool main;
    
public:
    SemanticAnalyser();

    void analyse(const std::vector<nodes::Node*>& ast);
private:
    //structures
    void visit(nodes::FuncDecl* funcDecl);
    void visit(nodes::VarDecl* varDecl);

    //keywords
    void visit(nodes::Abort* abort);

    //statement tree
    void visit(nodes::Binary* binary);
    void visit(nodes::VarRef* varRef);
};