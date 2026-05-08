#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "symbol.hpp"
#include <variant>

enum class NodeType
{
    funcDecl,
    varDecl,
    varRef,
    kwAbort,
    literal,
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
        Primitive returnType;
        std::vector<Node*> body;
        Function* symbol;

        FuncDecl(const std::vector<Node*>& body, std::string_view name, Primitive returnType, const Coordinate& location);
        ~FuncDecl() override;

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct VarDecl: public Node
    {
        std::string_view name;
        Primitive varType;
        Variable* symbol;

        VarDecl(std::string_view name, Primitive varType, const Coordinate& location);

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

    struct Literal: public Node
    {
        Primitive litType;
        std::variant<int> value;

        template <typename T>
        Literal(T value, const Coordinate& location):value(value)
        {
            type = NodeType::literal;
            this->location = location;

            if(std::is_same_v<T, int>)
            {
                litType = Primitive::intTp;
            }
        }

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct Binary: public Node{
        Primitive overallType;
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