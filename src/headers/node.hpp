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

        virtual std::string printToFile(int indentNum = 0, int space = 0, bool last = false) = 0;
    };

    struct Error: public Node
    {
        std::string error;

        Error(const std::string& error);

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct FuncDecl: public Node
    {
        std::string_view name;
        TokenType returnType;
        std::vector<Node*> body;

        FuncDecl(const std::vector<Node*>& body, std::string_view name, TokenType returnType, const Coordinate& location);
        ~FuncDecl() override;

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct VarDecl: public Node
    {
        std::string_view name;
        TokenType varType;
        uint id;

        VarDecl(std::string_view name, TokenType varType, uint id, const Coordinate& location);

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct VarRef: public Node
    {
        std::string_view name;
        uint id;

        VarRef(std::string_view name, const Coordinate& location);

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct Abort: public Node
    {
        Node* expression;  

        Abort(Node* expression, const Coordinate& location);
        ~Abort() override;

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct IntLit: public Node
    {
        int value;

        IntLit(int value, const Coordinate& location);

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct Binary: public Node{
        std::string op;
        Node* left;
        Node* right;

        Binary(std::string op, Node* left, Node* right, const Coordinate& location);
        ~Binary() override;

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct Empty: public Node
    {
        Empty();

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };
}