#include "node.hpp"

namespace nodes
{
    //Function Declaration
    FuncDecl::FuncDecl(const std::vector<Node*>& body):body(body)
    {
        type = NodeType::funcDecl;
    }

    FuncDecl::~FuncDecl()
    {
        for (Node* node: body)
        {
            if(node)
            {
                delete node;
            }
        }

        body.clear();
    }

    //Abort keyword
    Abort::Abort(Node* expression):expression(expression)
    {
        type = NodeType::kwAbort;
    }

    Abort::~Abort()
    {
        if(expression)
        {
            delete expression;
        }
    }

    //Integer literal
    IntLit::IntLit(int value):value(value)
    {
        type = NodeType::ltInt;
    }
}