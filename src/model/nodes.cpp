#include "nodes.hpp"

namespace nodes
{
    FunctionDecl::FunctionDecl(std::string returnType, std::string name, std::vector<std::unique_ptr<Node>> body): returnType(std::move(returnType)), name(std::move(name)), body(std::move(body))
    {
        type = NodeType::function;
    }

    Exit::Exit(std::unique_ptr<Node> expr): 
    expr(std::move(expr))
    {
        type = NodeType::exit;
    }

    Int::Int(int value):value(value)
    {
        type = NodeType::intLit;
    }

    Empty::Empty()
    {
        type = NodeType::empty;
    }
}