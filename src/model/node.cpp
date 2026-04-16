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
    FuncDecl::FuncDecl(const std::vector<Node*>& body, std::string_view name, TokenType returnType, const Coordinate& location):body(body), name(name), returnType(returnType)
    {
        type = NodeType::funcDecl;
        this->location = location;
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
    Abort::Abort(Node* expression, const Coordinate& location):expression(expression)
    {
        type = NodeType::kwAbort;
        this->location = location;
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
    IntLit::IntLit(int value, const Coordinate& location):value(value)
    {
        type = NodeType::ltInt;
        this->location = location;
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