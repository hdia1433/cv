#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "nodeType.hpp"

namespace nodes
{
    struct Node
    {
        NodeType type;

        virtual ~Node() = default;
    };

    struct FunctionDecl: public Node
    {
        std::string returnType;
        std::string name;
        std::vector<std::unique_ptr<Node>> body;

        FunctionDecl(std::string, std::string, std::vector<std::unique_ptr<Node>>);
    };


    struct Exit: public Node
    {
        std::unique_ptr<Node> expr;

        Exit(std::unique_ptr<Node>);
    };

    struct Int: public Node
    {
        int value;
    
        Int(int);
    };

    struct Empty: public Node
    {
        Empty();
    };
}