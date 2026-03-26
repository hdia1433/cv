#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "nodeType.hpp"
#include <unordered_map>

namespace nodes
{
    struct Node
    {
        NodeType type;

        virtual ~Node() = default;
    };

    struct Symbol: public Node
    {
        std::string type;
        int stackOff;
        int size;

        Symbol(const std::string& type, const int& stackOff, const int& size);
    };

    struct Scope: public Node
    {
        std::unordered_map<std::string, Symbol*> symbols;
    };

    struct FunctionDecl: public Node
    {
        std::string returnType;
        std::string name;
        std::vector<std::unique_ptr<Node>> body;

        FunctionDecl(std::string, std::string, std::vector<std::unique_ptr<Node>>);
    };

    struct VarDecl: public Node
    {
        std::string type;
        std::string name;
        std::unique_ptr<nodes::Node> value;
        int offset;
        int size;

        VarDecl(const std::string& type, const std::string& name, std::unique_ptr<nodes::Node> value, const int& offset, const int& size);
    };

    struct VarRef: public Node
    {
        std::string name;
        Symbol* symbol;

        VarRef(std::string, Symbol* symbol);
    };

    struct Abort: public Node
    {
        std::unique_ptr<Node> expr;

        Abort(std::unique_ptr<Node>);
    };

    struct Int: public Node
    {
        int value;
    
        Int(int = 0);
    };

    struct Empty: public Node
    {
        Empty();
    };
}