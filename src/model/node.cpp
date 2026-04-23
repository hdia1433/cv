#include "node.hpp"

namespace nodes
{
    std::string typeToString(TokenType type)
    {
        switch(type)
        {
            case TokenType::kwVoid:
                return "void";
            case TokenType::kwInt:
                return "int";
            default:
                return "error";
        }
    }
    
    //Error
    Error::Error(const std::string& error):error(std::move(error))
    {

    }

    void Error::print(const std::string& indent)
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

    void FuncDecl::print(const std::string& indent)
    {
        std::println("{}FunctionDeclaration({}, {}):", indent, name, typeToString(returnType));
        for (Node* node: body)
        {
            node->print(indent + "\t");
        }
    }

    //Variable declaration
    VarDecl::VarDecl(std::string_view name, TokenType varType, Node* value, const Coordinate& location):name(name), varType(varType), value(value)
    {
        type = NodeType::varDecl;
        this->location = location;
    }

    VarDecl::~VarDecl()
    {
        delete value;
    }

    void VarDecl::print(const std::string& indent)
    {
        std::println("{}VariableDeclaration({}, {}):", indent, name, typeToString(varType));
        value->print("\t" + indent);
    }

    //Variable reference
    VarRef::VarRef(std::string_view name, const Coordinate& location):name(name)
    {
        type = NodeType::varRef;
        this->location = location;
    }

    void VarRef::print(const std::string& indent)
    {
        std::println("{}VariableReference({})", indent, name);
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

    void Abort::print(const std::string& indent)
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

    void IntLit::print(const std::string& indent)
    {
        std::println("{}Integer Literal({})", indent, value);
    }

    //binary expression
    Binary::Binary(std::string op, Node* left, Node* right, const Coordinate& location): op(op), left(left), right(right)
    {
        type = NodeType::binary;
        this->location = location;
    }

    Binary::~Binary()
    {
        delete left;
        delete right;
    }

    void Binary::print(const std::string& indent)
    {
        std::println("{}Binary Operation:", indent);
        left->print(indent + "\t");
        std::println("{}{}", indent + "\t", op);
        right->print(indent + "\t");
    }

    //Empty
    Empty::Empty()
    {
        type = NodeType::empty;
    }

    void Empty::print(const std::string& indent)
    {
        std::println("{}Empty", indent);
    }
}