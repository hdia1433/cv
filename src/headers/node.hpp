#pragma once
#include "pch.hpp"
#include "token.hpp"
#include "symbol.hpp"
#include "type.hpp"
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
        Type returnType;
        std::vector<Node*> body;
        Function* symbol;

        FuncDecl(const std::vector<Node*>& body, std::string_view name, Type returnType, const Coordinate& location);
        ~FuncDecl() override;

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct VarDecl: public Node
    {
        std::string_view name;
        Type varType;
        Variable* symbol;

        VarDecl(std::string_view name, Type varType, const Coordinate& location);

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct VarRef: public Node
    {
        std::string_view name;
        Symbol* symbol;

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
        Type litType;
        std::variant<int, char> value;

        template <typename T>
        Literal(T value, const Coordinate& location):value(value)
        {
            type = NodeType::literal;
            this->location = location;

            if(std::is_same_v<T, int>)
            {
                litType = Type{.kind = TypeKind::tpInt};
            }
            else if(std::is_same_v<T, char>)
            {
                litType = Type{.kind = TypeKind::tpChar};
            }
        }

        std::string printToFile(int indentNum = 0, int space = 0, bool last = false) override;
    };

    struct Binary: public Node{
        Type overallType;
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