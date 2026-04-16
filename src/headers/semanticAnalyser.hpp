#pragma once
#include "pch.hpp"
#include "node.hpp"

class SemanticAnalyser
{
private:
    std::vector<std::string> errors;

    
public:
    SemanticAnalyser();

    void analyse(const std::vector<nodes::Node*>& ast);
private:
    //structures
    void visit(nodes::FuncDecl* funcDecl);

    //keywords
    void visit(nodes::Abort* abort);
};