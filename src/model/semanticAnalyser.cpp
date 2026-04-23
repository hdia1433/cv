#include "semanticAnalyser.hpp"
#include <sstream>

SemanticAnalyser::SemanticAnalyser():main(false)
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

    if(!main)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred.\nThe entry point was not found. A void function named \"main\" must be created as the entry point.";
        errors.insert(errors.begin(), errorStream.str());
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
    if(funcDecl->name == "main" && funcDecl->returnType == TokenType::kwVoid)
    {
        if(main)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << funcDecl->location.row << " and the column " << funcDecl->location.col << ".\nThe main function has already been declared. Only 1 entry point can be defined per project.";
            errors.emplace_back(errorStream.str());
        }
        else
        {
            main = true;
        }
    }

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
                errors.emplace_back(errorStream.str());
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