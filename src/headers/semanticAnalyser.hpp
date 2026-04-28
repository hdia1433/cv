#pragma once
#include "pch.hpp"
#include "node.hpp"
#include <unordered_map>

struct VarInfo
{
    std::string_view name;
    uint id;
    Primitive type;
    Coordinate location;
};

using ScopeStack = std::vector<std::unordered_map<std::string, VarInfo>>;

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
    bool visit(nodes::FuncDecl* funcDecl);
    bool visit(nodes::VarDecl* varDecl);

    //keywords
    bool visit(nodes::Abort* abort);

    //statement tree
    bool visit(nodes::Binary* binary);
    bool visit(nodes::VarRef* varRef);

    //helper
    Primitive checkTypes(Primitive type1, Primitive type2);
};