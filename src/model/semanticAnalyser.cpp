#include "semanticAnalyser.hpp"
#include <sstream>

SemanticAnalyser::SemanticAnalyser()
{

}

void SemanticAnalyser::analyse(const std::vector<nodes::Node*>& ast)
{
    for(nodes::Node* node: ast)
    {
        switch(node->type)
        {
            case NodeType::funcDecl:
            {
                visit((nodes::FuncDecl*)node);
                break;
            }
            default:
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << node->location.row << " and the column " << node->location.col << ".\nA function declaration or a variable declaration was expected.";
                errors.emplace_back(errorStream.str());
                break;
            }
        }
    }

    if(errors.size() > 0)
    {
        for(std::string error: errors)
        {
            std::cerr << error << std::endl << std::endl;
        }

        std::stringstream errorStream;
        errorStream << errors.size() << " errors were generated.\n";

        throw std::runtime_error(errorStream.str());
    }
}

void SemanticAnalyser::visit(nodes::FuncDecl* funcDecl)
{
    for(nodes::Node* node: funcDecl->body)
    {
        switch(node->type)
        {
            case NodeType::kwAbort:
            {
                visit((nodes::Abort*)node);
                break;
            }
            default:
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << node->location.row << " and the column " << node->location.col << ".\nA statement was expected.";
            }
        }
    }
}

void SemanticAnalyser::visit(nodes::Abort* abort)
{
    switch(abort->expression->type)
    {
        case NodeType::binary:
            visit((nodes::Binary*)abort->expression);
            break;
        default:
            break;
    }
}

void SemanticAnalyser::visit(nodes::Binary* binary)
{
    
}