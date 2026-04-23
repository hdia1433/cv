#include "node.hpp"
#include <sstream>

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

    std::string Error::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;
        result << indent << "Error(" << error << ")\n";
        return result.str();
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

    std::string FuncDecl::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Function Declaration(" << name << ", " << typeToString(returnType) << "):\n";
        for (Node* node: body)
        {
            result << node->printToFile(indentNum + 1);
        }

        return result.str();
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

    std::string VarDecl::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Variable Declaration(" << name << ", " << typeToString(varType) << "):\n";
        result << value->printToFile(indentNum + 1);

        return result.str();
    }

    //Variable reference
    VarRef::VarRef(std::string_view name, const Coordinate& location):name(name)
    {
        type = NodeType::varRef;
        this->location = location;
    }

    std::string VarRef::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Variable Reference(" << name << ")\n";

        return result.str();
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

    std::string Abort::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Abort:\n";
        result << expression->printToFile(indentNum + 1);

        return result.str();
    }

    //Integer literal
    IntLit::IntLit(int value, const Coordinate& location):value(value)
    {
        type = NodeType::ltInt;
        this->location = location;
    }

    std::string IntLit::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Integer Literal(" << value << ")\n";

        return result.str();
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

    std::string Binary::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        result << indent << "Binary Operation:\n";
        result << left->printToFile(indentNum + 1);
        result << "| " << indent << op << std::endl;
        result << right->printToFile(indentNum + 1);

        return result.str();
    }

    //Empty
    Empty::Empty()
    {
        type = NodeType::empty;
    }

    std::string Empty::printToFile(int indentNum)
    {
        std::string indent("| ", indentNum);
        if(!indent.empty())
        {
            indent[indent.size() - 1] = '-';
        }
        std::stringstream result;

        std::println("{}Empty", indent);
        result << indent << "Empty\n";

        return result.str();
    }
}