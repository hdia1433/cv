#include "node.hpp"

namespace nodes
{
    //Error
    Error::Error(const std::string& error):error(std::move(error))
    {

    }

    void Error::print(std::string indent)
    {
        std::println("{}Error({})", indent, error);
    }

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

    void FuncDecl::print(std::string indent)
    {
        std::println("{}FunctionDeclaration:", indent);
        for (Node* node: body)
        {
            node->print(indent + "\t");
        }
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

    void Abort::print(std::string indent)
    {
        std::println("{}Abort:", indent);
        expression->print(indent + "\t");
    }

    //Integer literal
    IntLit::IntLit(int value):value(value)
    {
        type = NodeType::ltInt;
    }

    void IntLit::print(std::string indent)
    {
        std::println("{}Integer Literal({})", indent, value);
    }

    //Empty
    Empty::Empty()
    {
        type = NodeType::empty;
    }

    void Empty::print(std::string indent)
    {
        std::println("{}Empty", indent);
    }
}