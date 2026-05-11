#include "semanticAnalyser.hpp"
#include <sstream>
#include <ranges>
#include "symbol.hpp"
#include "helpers.hpp"

SemanticAnalyser::SemanticAnalyser():main(false)
{
    scopeStack.emplace_back();
}

void SemanticAnalyser::analyse(const std::vector<nodes::Node*>& ast)
{
    bool noError = true;

    currentSymbolTable = &globalScope.variables;

    for(nodes::Node* node: ast)
    {
        switch(node->type)
        {
            case NodeType::funcDecl:
            {
                noError &= visit((nodes::FuncDecl*)node);
                break;
            }
            case NodeType::varDecl:
            {
                nodes::VarDecl* varDecl = ((nodes::VarDecl*)node);
                varDecl->symbol->global = true;
                noError &= visit(varDecl);
                break;
            }
            case NodeType::binary:
                noError &= visit((nodes::Binary*)node);
                break;
            default:
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << node->location.row << " and the column " << node->location.col << ".\nA type for a function declaration or a variable declaration was expected, but ,'";
                errors.emplace_back(errorStream.str());
                noError = false;
            }
        }
    }

    if(!main)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred.\nThe entry point was not found. A void function named \"main\" must be created as the entry point.";
        errors.insert(errors.begin(), errorStream.str());
        noError = false;
    }

    if(errors.size() > 0)
    {
        for(std::string error: errors)
        {
            std::cerr << error << std::endl << std::endl;
        }

        std::stringstream errorStream;
        if(errors.size() > 1)
        {
            errorStream << errors.size() << " errors were generated.\n";
        }
        else
        {
            errorStream << "1 error was generated.\n";
        }

        throw std::runtime_error(errorStream.str());
    }
}

bool SemanticAnalyser::visit(nodes::FuncDecl* funcDecl)
{
    bool result = true;

    scopeStack.emplace_back();

    if(funcDecl->name == "main" && funcDecl->returnType == Primitive::voidTp)
    {
        if(main)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << funcDecl->location.row << " and the column " << funcDecl->location.col << ".\nThe main function has already been declared. Only 1 entry point can be defined per project.";
            errors.emplace_back(errorStream.str());
            result = false;
        }
        else
        {
            main = true;
        }
    }

    if(globalScope.functions.find(FunctionSignature{.name = std::string(funcDecl->name), .returnType = funcDecl->returnType}) != globalScope.functions.end())
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << funcDecl->location.row << " and the column " << funcDecl->location.col << ".\nA function has already been declared with the same signature as the function: " << funcDecl->name << ". Only 1 function can be declared with 1 signature per scope.";
        errors.emplace_back(errorStream.str());
        result = result && false;
    }

    Function* symbol = new Function(std::string(funcDecl->name), funcDecl->returnType, funcDecl->location);
    funcDecl->symbol = symbol;
    globalScope.functions.emplace(FunctionSignature{.name = std::string(funcDecl->name), .returnType = funcDecl->returnType}, symbol);

    auto oldSymbolTable = currentSymbolTable;
    currentSymbolTable = &symbol->locals;

    for(nodes::Node* node: funcDecl->body)
    {
        bool result = true;

        switch(node->type)
        {
            case NodeType::kwAbort:
            {
                result &= visit((nodes::Abort*)node);
                break;
            }
            case NodeType::varDecl:
                result &= visit((nodes::VarDecl*)node);
                break;
            case NodeType::binary:
                result &= visit((nodes::Binary*)node);
                break;
            default:
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << node->location.row << " and the column " << node->location.col << ".\nA valid statement was expected.";
                errors.emplace_back(errorStream.str());
                result = false;
            }
        }
    }

    scopeStack.pop_back();
    currentSymbolTable = oldSymbolTable;
    
    return result;
}

bool SemanticAnalyser::visit(nodes::VarDecl* varDecl)
{
    for(const auto& [key, value]: scopeStack.back())
    {
        if(key == varDecl->name)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << varDecl->location.row << " and the column " << varDecl->location.col << ".\nA variable with the name \"" << varDecl->name << "\" has already been declared at the line " << value->location.row << ".\nA variable cannot have the same name as another variable already declared within the same scope.";
            errors.emplace_back(errorStream.str());
            return false;
        }
    }

    Variable* variable = new Variable(std::string(varDecl->name), varDecl->varType, varDecl->location);

    scopeStack.back().emplace(varDecl->name, variable);
    varDecl->symbol = variable;
    currentSymbolTable->emplace(variable->name, variable);

    return true;
}

bool SemanticAnalyser::visit(nodes::VarRef* varRef)
{
    for(auto& scope: scopeStack | std::views::reverse)
    {
        auto symbol = scope.find(std::string(varRef->name));
        if(symbol != scope.end())
        {
            varRef->symbol = symbol->second;
            return true;
        }
    }

    std::stringstream errorStream;
    errorStream << "An error has occurred at the line " << varRef->location.row << " and the column " << varRef->location.col << ".\nA variable of this name has not been defined. A variable cannot be used before it is defined.";
    errors.emplace_back(errorStream.str());
    return false;
}

bool SemanticAnalyser::visit(nodes::Abort* abort)
{
    switch(abort->expression->type)
    {
        case NodeType::binary:
            return visit((nodes::Binary*)abort->expression);
        case NodeType::varRef:
            return visit((nodes::VarRef*)abort->expression);
        case NodeType::literal:
        {
            nodes::Literal* literal = (nodes::Literal*)abort->expression;
            if(checkTypes(literal->litType, {Primitive::intTp}) != Primitive::custom)
            {
                return true;
            }
            break;
        }
        default:
            break;
    }
    
    return false;
}

bool SemanticAnalyser::visit(nodes::Binary* binary)
{
    bool result = true;

    if(binary->op == "=")
    {
        Primitive type1;

        switch(binary->left->type)
        {
            case NodeType::varDecl:
            {
                nodes::VarDecl* varDecl = (nodes::VarDecl*)binary->left;
                if(visit(varDecl))
                {
                    type1 = varDecl->varType;
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::varRef:
            {
                nodes::VarRef* varRef = (nodes::VarRef*)binary->left;
                if(!visit(varRef))
                {
                    result = false;
                }
                else
                {
                    for(auto& scope: scopeStack | std::views::reverse)
                    {
                        auto varInfo = scope.find(std::string(varRef->name));
                        if(varInfo != scope.end())
                        {
                            type1 = varInfo->second->type;
                            break;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }

        Primitive type2;

        type2 = Primitive::custom;

        switch(binary->right->type)
        {
            case NodeType::binary:
            {
                auto innerBinary = (nodes::Binary*)binary->right;
                if(visit(innerBinary))
                {
                    type2 = innerBinary->overallType;
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::varRef:
            {
                auto varRef = (nodes::VarRef*)binary->right;
                if(visit(varRef))
                {
                    for(auto& scope: scopeStack | std::views::reverse)
                    {
                        auto variable = scope.find(std::string(varRef->name));
                        if(variable != scope.end())
                        {
                            type2 = variable->second->type;
                            break;
                        }
                    }
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::literal:
            {
                auto literal = (nodes::Literal*)binary->right;
                type2 = literal->litType;
                break;
            }
            default:
                break;
        }

        binary->overallType = checkTypes(type1, type2);
    }
    else if(helpers::equalsOr<std::string>(binary->op, {"+", "-"}))
    {
        Primitive type1;

        switch(binary->right->type)
        {
            case NodeType::binary:
            {
                auto innerBinary = (nodes::Binary*)binary->right;
                if(visit(innerBinary))
                {
                    type1 = innerBinary->overallType;
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::varRef:
            {
                auto varRef = (nodes::VarRef*)binary->right;
                if(visit(varRef))
                {
                    for(auto& scope: scopeStack | std::views::reverse)
                    {
                        auto variable = scope.find(std::string(varRef->name));
                        if(variable != scope.end())
                        {
                            type1 = variable->second->type;
                            break;
                        }
                    }
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::literal:
            {
                auto literal = (nodes::Literal*)binary->right;
                type1 = literal->litType;
                break;
            }
            default:
                break;
        }

        Primitive type2;

        switch(binary->right->type)
        {
            case NodeType::binary:
            {
                auto innerBinary = (nodes::Binary*)binary->right;
                if(visit(innerBinary))
                {
                    type2 = innerBinary->overallType;
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::varRef:
            {
                auto varRef = (nodes::VarRef*)binary->right;
                if(visit(varRef))
                {
                    for(auto& scope: scopeStack | std::views::reverse)
                    {
                        auto variable = scope.find(std::string(varRef->name));
                        if(variable != scope.end())
                        {
                            type2 = variable->second->type;
                            break;
                        }
                    }
                }
                else
                {
                    result = false;
                }
                break;
            }
            case NodeType::literal:
            {
                auto literal = (nodes::Literal*)binary->right;
                type2 = literal->litType;
                break;
            }
            default:
                break;
        }

        binary->overallType = checkTypes(type1, type2);
    }
    
    return result;
}

#pragma region helpers
Primitive SemanticAnalyser::checkTypes(Primitive type1, Primitive type2)
{
    if(type1 == type2)
    {
        return type1;
    }

    return Primitive::custom;
}

Primitive SemanticAnalyser::checkTypes(Primitive type, std::initializer_list<Primitive> others)
{
    for(Primitive other: others)
    {
        if(type == other)
        {
            return type;
        }
    }

    return Primitive::custom;
}
#pragma endregion