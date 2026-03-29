#include "nodes.hpp"

namespace nodes
{
    Symbol::Symbol(const std::string& type, const int& stackOff, const int& size):type(std::move(type)), stackOff(std::move(stackOff)), size(std::move(size))
    {
        Node::type = NodeType::symbol;
    }

    FunctionDecl::FunctionDecl(std::string returnType, std::string name, std::vector<std::unique_ptr<Node>> body): returnType(std::move(returnType)), name(std::move(name)), body(std::move(body))
    {
        type = NodeType::functionDecl;
    }

    VarDecl::VarDecl(const std::string& type, const std::string& name, std::unique_ptr<Node> value, const int& offset, const int& size): type(std::move(type)), name(std::move(name)), value(std::move(value)), offset(std::move(offset)), size(std::move(size))
    {
        Node::type = NodeType::varDecl;
    }

    VarRef::VarRef(std::string name, Symbol* symbol):name(std::move(name)), isGlobal(false), symbol(symbol)
    {
        type = NodeType::varRef;
    }

    VarRef::VarRef(std::string name, Global* global):name(std::move(name)), isGlobal(true), global(global)

    Abort::Abort(std::unique_ptr<Node> expr): 
    expr(std::move(expr))
    {
        type = NodeType::abort;
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