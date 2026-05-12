#pragma once
#include "pch.hpp"
#include "node.hpp"
#include "symbol.hpp"
#include <unordered_map>

struct VarInfo
{
    std::string_view name;
    uint id;
    Primitive type;
    Coordinate location;
};

using ScopeStack = std::vector<std::unordered_map<std::string, Variable*>>;

class SemanticAnalyser
{
private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    ScopeStack scopeStack;
    std::unordered_map<std::string, Variable*>* currentSymbolTable;
    bool main;
    
public:
    SemanticAnalyser();

    void analyse(const std::vector<nodes::Node*>& ast);
private:
    //structures
    bool visit(nodes::FuncDecl* funcDecl);
    bool visit(nodes::VarDecl* varDecl, bool global = false);

    //keywords
    bool visit(nodes::Abort* abort);

    //statement tree
    bool visit(nodes::Binary* binary, bool global = false);
    bool visit(nodes::VarRef* varRef);

    //helper
    Primitive checkTypes(Primitive type1, Primitive type2);
    Primitive checkTypes(Primitive type1, std::initializer_list<Primitive> others);
};