#include "node.hpp"
#include <sstream>

namespace nodes
{
Node::Node(NodeType type, Coordinate location): type(type), location(location)
{
}

// Error
Error::Error(const std::string& error): Node(NodeType::empty, Coordinate{}), error(std::move(error))
{
}

std::string Error::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;
    result << indent << "Error(" << error << ")\n";
    return result.str();
}

// Function Declaration
FuncDecl::FuncDecl(const std::vector<Node*>& body, std::string_view name, Type returnType, const Coordinate& location):
    Node(NodeType::funcDecl, location),
    body(body),
    name(name),
    returnType(std::move(returnType))
{
}

FuncDecl::~FuncDecl()
{
    for (Node* node : body)
    {
        if (node)
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
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Function Declaration(" << name << ", " << returnType << "):\n";
    for (uint i = 0; i < body.size(); i++)
    {
        if (i == body.size() - 1)
        {
            result << body[i]->printToFile(indentNum + 1, space, true);
            break;
        }
        result << body[i]->printToFile(indentNum + 1, space);
    }

    return result.str();
}

// Variable declaration
VarDecl::VarDecl(std::string_view name, Type varType, const Coordinate& location):
    Node(NodeType::varDecl, location),
    name(name),
    varType(std::move(varType))
{
}

std::string VarDecl::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Variable Declaration(" << name << ", " << varType << "):\n";

    return result.str();
}

// Variable reference
VarRef::VarRef(std::string_view name, const Coordinate& location): Node(NodeType::varRef, location), name(name)
{
}

std::string VarRef::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Variable Reference(" << name << ")\n";

    return result.str();
}

// Abort keyword
Abort::Abort(Node* expression, const Coordinate& location): Node(NodeType::kwAbort, location), expression(expression)
{
}

Abort::~Abort()
{
    if (expression)
    {
        delete expression;
    }
}

std::string Abort::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Abort:\n";
    result << expression->printToFile(indentNum + 1, space, true);

    return result.str();
}

// literal
std::string Literal::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    if (int* integer = std::get_if<int>(&value))
    {
        result << indent << "Integer Literal(" << *integer;
    }
    else if (char* character = std::get_if<char>(&value))
    {
        result << indent << "Character Literal(" << *character;
    }

    result << ")\n";

    return result.str();
}

InitList::InitList(const std::vector<Node*>& values, const Coordinate& location):
    Node(NodeType::initList, location),
    values(values)
{
}

InitList::~InitList()
{
    for (Node* node : values)
    {
        delete node;
    }
}

std::string InitList::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Initialiser list:\n";

    for (uint i = 0; i < values.size(); i++)
    {
        last = (i == values.size() - 1);

        indent = "";
        indent.reserve((indentNum + 1) * 2);
        if (indentNum + 1 > 0)
        {
            for (uint j = 0; j < indentNum; j++)
            {
                if ((space >> j) & 1)
                {
                    indent += "    ";
                }
                else
                {
                    indent += "│   ";
                }
            }
            if (last)
            {
                indent += "└── ";
            }
            else
            {
                indent += "├── ";
            }
        }

        if (i == values.size() - 1)
        {
            space |= (1 << (indentNum - 1));
        }

        result << indent;
        result << values[i]->printToFile();
    }

    return result.str();
}

// binary expression
Binary::Binary(std::string op, Node* left, Node* right, const Coordinate& location):
    Node(NodeType::binary, location),
    op(op),
    left(left),
    right(right)
{
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
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Binary Operation:\n";
    result << left->printToFile(indentNum + 1, space);
    std::string newIndent = "";
    int newIndentNum = indentNum + 1;
    newIndent.reserve(newIndentNum * 2);
    if (newIndentNum > 0)
    {
        for (uint i = 0; i < newIndentNum - 1; i++)
        {
            if ((space >> i) & 1)
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

Unary::Unary(const std::string& op, Node* expression, const Coordinate& location):
    Node(NodeType::unary, location),
    op(op),
    expression(expression)
{
}

std::string Unary::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Unary Operation:\n";
    std::string newIndent = "";
    int newIndentNum = indentNum + 1;
    newIndent.reserve(newIndentNum * 2);
    if (newIndentNum > 0)
    {
        for (uint i = 0; i < newIndentNum - 1; i++)
        {
            if ((space >> i) & 1)
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
    result << expression->printToFile(indentNum + 1, space, true);

    return result.str();
}

//subscript
Subscript::Subscript(Node* expression, Node* index, const Coordinate& location): Node(NodeType::subscript, location), expression(expression), index(index)
{

}

std::string Subscript::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Subscript:\n";
    result << expression->printToFile(indentNum + 1, space, false);
    std::string newIndent = "";
    int newIndentNum = indentNum + 1;
    newIndent.reserve(newIndentNum * 2);
    if (newIndentNum > 0)
    {
        for (uint i = 0; i < newIndentNum - 1; i++)
        {
            if ((space >> i) & 1)
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
    result << newIndent << "[\n";
    result << index->printToFile(indentNum + 1, space, false);
    newIndent = newIndent.substr(0, newIndent.find("├")) + "└── ";
    result << newIndent << "]\n";

    return result.str();
}


// Empty
Empty::Empty(): Node(NodeType::empty, Coordinate{})
{
}

std::string Empty::printToFile(int indentNum, int space, bool last)
{
    std::string indent = "";
    indent.reserve(indentNum * 2);
    if (indentNum > 0)
    {
        for (uint i = 0; i < indentNum - 1; i++)
        {
            if ((space >> i) & 1)
            {
                indent += "    ";
            }
            else
            {
                indent += "│   ";
            }
        }
        if (last)
        {
            indent += "└── ";
        }
        else
        {
            indent += "├── ";
        }
    }
    if (last && indentNum > 0)
    {
        space |= (1 << (indentNum - 1));
    }
    std::stringstream result;

    result << indent << "Empty\n";

    return result.str();
}
} // namespace nodes
