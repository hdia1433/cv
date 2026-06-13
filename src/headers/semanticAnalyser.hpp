#pragma once
#include "node.hpp"
#include "type.hpp"

struct VarInfo
{
    std::string_view name;
    uint id;
    Type type;
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
    // structures
    bool visit(nodes::FuncDecl* funcDecl);
    bool visit(nodes::VarDecl* varDecl, bool global = false);

    // keywords
    bool visit(nodes::Abort* abort);

    // statement tree
    bool visit(nodes::Binary* binary, bool global = false);
    bool visit(nodes::Unary* unary);
    bool visit(nodes::Subscript* subscript);
    bool visit(nodes::VarRef* varRef);
    bool visit(nodes::InitList* initList);

    // helper
    Type checkTypes(Type type1, Type type2);
    Type checkTypes(Type type1, std::initializer_list<Type> others);
};
