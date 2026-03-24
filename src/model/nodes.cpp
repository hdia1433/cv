#include "nodes.hpp"

namespace nodes
{
    Symbol::Symbol(std::string type, int stackOff):type(std::move(type)), stackOff(std::move(stackOff))
    {
        Node::type = NodeType::symbol;
    }

    FunctionDecl::FunctionDecl(std::string returnType, std::string name, std::vector<std::unique_ptr<Node>> body): returnType(std::move(returnType)), name(std::move(name)), body(std::move(body))
    {
        type = NodeType::functionDecl;
    }

    VarDecl::VarDecl(std::string type, std::string name, std::unique_ptr<Node> value): type(std::move(type)), name(std::move(name)), value(std::move(value))
    {
        Node::type = NodeType::varDecl;
    }

    VarRef::VarRef(std::string name, Symbol* symbol):name(std::move(name)), symbol(symbol)
    {
        type = NodeType::varRef;
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