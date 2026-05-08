#include "node.hpp"
#include <sstream>

namespace nodes
{
    std::string typeToString(Primitive type)
    {
        switch(type)
        {
            case Primitive::voidTp:
                return "void";
            case Primitive::intTp:
                return "int";
            default:
                return "error";
        }
    }
    
    //Error
    Error::Error(const std::string& error):error(std::move(error))
    {

    }

    std::string Error::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;
        result << indent << "Error(" << error << ")\n";
        return result.str();
    }

    //Function Declaration
    FuncDecl::FuncDecl(const std::vector<Node*>& body, std::string_view name, Primitive returnType, const Coordinate& location):body(body), name(name), returnType(returnType)
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

    std::string FuncDecl::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Function Declaration(" << name << ", " << typeToString(returnType) << "):\n";
        for (uint i = 0; i < body.size(); i++)
        {
            if(i == body.size() - 1)
            {
                result << body[i]->printToFile(indentNum + 1, space, true);
                break;
            }
            result << body[i]->printToFile(indentNum + 1, space);
        }

        return result.str();
    }

    //Variable declaration
    VarDecl::VarDecl(std::string_view name, Primitive varType, const Coordinate& location):name(name), varType(varType)
    {
        type = NodeType::varDecl;
        this->location = location;
    }

    std::string VarDecl::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Variable Declaration(" << name << ", " << typeToString(varType) << "):\n";

        return result.str();
    }

    //Variable reference
    VarRef::VarRef(std::string_view name, const Coordinate& location):name(name)
    {
        type = NodeType::varRef;
        this->location = location;
    }

    std::string VarRef::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
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

    std::string Abort::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Abort:\n";
        result << expression->printToFile(indentNum + 1, space, true);

        return result.str();
    }

    //Integer literal
    std::string Literal::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Integer Literal(";
        
        if(auto integer = std::get_if<int>(&value))
        {
            result << *integer;
        }
        
        result << ")\n";

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

    std::string Binary::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Binary Operation:\n";
        result << left->printToFile(indentNum + 1, space);
        std::string newIndent = "";
        int newIndentNum = indentNum + 1;
        newIndent.reserve(newIndentNum * 2);
        if(newIndentNum > 0)
        {
            for(uint i = 0; i < newIndentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    newIndent += "    ";
                }
                else
                {
                    newIndent += "│   ";
                }

            }
            
            newIndent += "├── ";
        }
        result << newIndent << op << std::endl;
        result << right->printToFile(indentNum + 1, space, true);

        return result.str();
    }

    //Empty
    Empty::Empty()
    {
        type = NodeType::empty;
    }

    std::string Empty::printToFile(int indentNum, int space, bool last)
    {
        std::string indent = "";
        indent.reserve(indentNum * 2);
        if(indentNum > 0)
        {
            for(uint i = 0; i < indentNum - 1; i++)
            {
                if((space >> i) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }

            }
            if(last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }
        if(last && indentNum > 0)
        {
            space |= (1 << (indentNum - 1));
        }
        std::stringstream result;

        result << indent << "Empty\n";

        return result.str();
    }
}