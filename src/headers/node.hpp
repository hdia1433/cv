#pragma once
#include "pch.hpp"
#include "token.hpp"

enum class NodeType
{
    funcDecl,
    kwAbort,
    ltInt,
    binary,
    empty
};

namespace nodes
{
    struct Node
    {
        NodeType type;
        Coordinate location;

        virtual ~Node() = default;

        virtual void print(std::string indent = "") = 0;
    };

    struct Error: public Node
    {
        std::string error;

        Error(const std::string& error);

        void print(std::string indent = "") override;
    };

    struct FuncDecl: public Node
    {
        std::string_view name;
        TokenType returnType;
        std::vector<Node*> body;

        FuncDecl(const std::vector<Node*>& body, std::string_view name, TokenType returnType, const Coordinate& location);
        ~FuncDecl() override;

        void print(std::string indent = "") override;
    };

    struct Abort: public Node
    {
        Node* expression;  

        Abort(Node* expression, const Coordinate& location);
        ~Abort() override;

        void print(std::string indent = "") override;
    };

    struct IntLit: public Node
    {
        int value;

        IntLit(int value, const Coordinate& location);

        void print(std::string indent = "") override;
    };

    struct Binary: public Node{
        std::string op;
        Node* left;
        Node* right;

        Binary(std::string op, Node* left, Node* right);
        ~Binary() override;

        void print(std::string indent = "") override;
    };

    struct Empty: public Node
    {
        Empty();

        void print(std::string indent = "") override;
    };
}