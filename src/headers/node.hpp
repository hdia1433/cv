#include "pch.hpp"
#include "token.hpp"

enum class NodeType
{
    funcDecl,
    kwAbort,
    ltInt
};

namespace nodes
{
    struct Node
    {
        NodeType type;
        Coordinate location;

        Coordinate operator->();
    };

    struct Error: public Node
    {
        std::string error;
    };

    struct FuncDecl: public Node
    {
        std::vector<Node*> body;

        FuncDecl(const std::vector<Node*>& body);
        ~FuncDecl();
    };

    struct Abort: public Node
    {
        Node* expression;  

        Abort(Node* expression);
        ~Abort();
    };

    struct IntLit: public Node
    {
        int value;

        IntLit(int value);
    };
}