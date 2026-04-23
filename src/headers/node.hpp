#pragma once
#include "pch.hpp"
#include "token.hpp"

enum class NodeType
{
    funcDecl,
    varDecl,
    varRef,
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

        virtual void print(const std::string& indent = "") = 0;
    };

    struct Error: public Node
    {
        std::string error;

        Error(const std::string& error);

        void print(const std::string& indent = "") override;
    };

    struct FuncDecl: public Node
    {
        std::string_view name;
        TokenType returnType;
        std::vector<Node*> body;

        FuncDecl(const std::vector<Node*>& body, std::string_view name, TokenType returnType, const Coordinate& location);
        ~FuncDecl() override;

        void print(const std::string& indent = "") override;
    };

    struct VarDecl: public Node
    {
        std::string_view name;
        TokenType varType;
        Node* value;

        VarDecl(std::string_view name, TokenType varType, Node* value, const Coordinate& location);
        ~VarDecl() override;

        void print(const std::string& indent = "") override;
    };

    struct VarRef: public Node
    {
        std::string_view name;

        VarRef(std::string_view name, const Coordinate& location);

        void print(const std::string& indent = "") override;
    };

    struct Abort: public Node
    {
        Node* expression;  

        Abort(Node* expression, const Coordinate& location);
        ~Abort() override;

        void print(const std::string& indent = "") override;
    };

    struct IntLit: public Node
    {
        int value;

        IntLit(int value, const Coordinate& location);

        void print(const std::string& indent = "") override;
    };

    struct Binary: public Node{
        std::string op;
        Node* left;
        Node* right;

        Binary(std::string op, Node* left, Node* right, const Coordinate& location);
        ~Binary() override;

        void print(const std::string& indent = "") override;
    };

    struct Empty: public Node
    {
        Empty();

        void print(const std::string& indent = "") override;
    };
}