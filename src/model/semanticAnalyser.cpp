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

    currentSymbolTable = &getGlobalScope().variables;

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
                noError &= visit(varDecl, true);
                break;
            }
            case NodeType::binary:
                noError &= visit((nodes::Binary*)node, true);
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

    if(!warnings.empty())
    {
        for(std::string warning: warnings)
        {
            std::cerr << warning << std::endl << std::endl;
        }

        if(warnings.size() > 1)
        {
            std::cerr << warnings.size() << " warnings were generated.\n";
        }
        else
        {
            std::cerr << "1 warning was generated.\n";
        }
    }
}

bool SemanticAnalyser::visit(nodes::FuncDecl* funcDecl)
{
    bool result = true;

    scopeStack.emplace_back();

    if(funcDecl->name == "main" && funcDecl->returnType.kind == TypeKind::tpVoid)
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

    if(getGlobalScope().functions.find(FunctionSignature{.name = std::string(funcDecl->name), .returnType = funcDecl->returnType}) != getGlobalScope().functions.end())
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << funcDecl->location.row << " and the column " << funcDecl->location.col << ".\nA function has already been declared with the same signature as the function: " << funcDecl->name << ". Only 1 function can be declared with 1 signature per scope.";
        errors.emplace_back(errorStream.str());
        result = result && false;
    }

    Function* symbol = new Function(std::string(funcDecl->name), funcDecl->returnType, funcDecl->location);
    funcDecl->symbol = symbol;
    getGlobalScope().functions.emplace(FunctionSignature{.name = std::string(funcDecl->name), .returnType = funcDecl->returnType}, symbol);

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

bool SemanticAnalyser::visit(nodes::VarDecl* varDecl, bool global)
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
    variable->global = global;

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

bool SemanticAnalyser::visit(nodes::InitList* initList)
{


    for(uint i = 0; i < initList->values.size(); i++)
    {
        nodes::Node* value = initList->values[i];

        switch(value->type)
        {
            case NodeType::literal:
            {
                nodes::Literal* literal = (nodes::Literal*)value;

                if(i == 0)
                {
                    initList->baseType = literal->litType;
                }
                else if(initList->baseType != literal->litType)
                {
                    std::stringstream errorStream;
                    errorStream << "An error has occurred at the line " << literal->location.row << " and the column " << literal->location.col << ".\nThe value " << literal->value << " is of the type " << literal->litType << " while the init list is of type " << initList->baseType << " (The type of the first value in the list.). There is no implicit conversion between those types.";
                    errors.emplace_back(errorStream.str());
                    return false;
                }
                break;
            }
            case NodeType::varRef:
            {
                nodes::VarRef* varRef = (nodes::VarRef*)value;
                auto symbol = currentSymbolTable->find(std::string(varRef->name));
                if(symbol == currentSymbolTable->end())
                {
                    std::stringstream errorStream;
                    errorStream << "An error has occurred at the line " << varRef->location.row << " and the column " << varRef->location.col << ".\nThe variable " << varRef->name << " is undefined. Variables must be declared before they are used.";
                    errors.emplace_back(errorStream.str());
                    return false;
                }
                else if(0 == i)
                {
                    initList->baseType = symbol->second->type;
                }
                else if(symbol->second->type != initList->baseType)
                {
                    std::stringstream errorStream;
                    errorStream << "An error has occurred at the line " << varRef->location.row << " and the column " << varRef->location.col << ".\nThe variable \"" << varRef->name << "\" is of the type " << symbol->second->type << " while the init list is of type " << initList->baseType << " (The type of the first value in the list.). There is no implicit conversion between those types.";
                    errors.emplace_back(errorStream.str());
                    return false;
                }
                break;
            }
            case NodeType::binary:
            {
                nodes::Binary* binary = (nodes::Binary*)value;

                if(0 == 1)
                {
                    initList->baseType = binary->overallType;
                }
                else if(initList->baseType != binary->overallType)
                {
                    std::stringstream errorStream;
                    errorStream << "An error has occurred at the line " << binary->location.row << " and the column " << binary->location.col << ".\nThe operation results in the type " << binary->overallType << " while the init list is of type " << initList->baseType << " (The type of the first value in the list.). There is no implicit conversion between those types.";
                    errors.emplace_back(errorStream.str());
                    return false;
                }
                break;
            }
            default:
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << value->location.row << " and the column " << value->location.col << ".\nA viable value to be put in an initialiser list was expected.\n";
                errors.emplace_back(errorStream.str());
                return false;
        }
    }
    
    return true;
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
            if(checkTypes(literal->litType, Type{.kind = TypeKind::tpInt}).kind != TypeKind::tpError)
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

bool SemanticAnalyser::visit(nodes::Binary* binary, bool global)
{
    bool result = true;
    Type type1;
    Type type2;

    if(binary->op == "=")
    {
        bool varNeedsSize = false;
        switch(binary->left->type)
        {
            case NodeType::varDecl:
            {
                nodes::VarDecl* varDecl = (nodes::VarDecl*)binary->left;
                if(visit(varDecl, global))
                {
                    type1 = varDecl->varType;
                    
                    if(TypeKind::tpArray == type1.kind && 0 == type1.size)
                    {
                        varNeedsSize = true;
                    }
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

        type2.kind = TypeKind::tpError;

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
            case NodeType::initList:
            {
                auto initList = (nodes::InitList*)binary->right;
                if(visit(initList))
                {
                    type2 = Type{.kind = TypeKind::tpArray, .baseType = &initList->baseType, .size = (int)initList->values.size()};

                    if(varNeedsSize)
                    {
                        ((nodes::VarDecl*)binary->left)->varType.size = type2.size;
                        type1.size = type2.size;
                    }
                }
                else
                {
                    type2 = Type{.kind = TypeKind::tpError};
                }
                break;
            }
            default:
                break;
        }

        binary->overallType = checkTypes(type1, type2);
    }
    else if(helpers::equalsOr<std::string>(binary->op, {"+", "-", "*"}))
    {
        switch(binary->left->type)
        {
            case NodeType::binary:
            {
                auto innerBinary = (nodes::Binary*)binary->left;
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
                auto varRef = (nodes::VarRef*)binary->left;
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
                auto literal = (nodes::Literal*)binary->left;
                type1 = literal->litType;
                break;
            }
            default:
                break;
        }

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

    if(binary->overallType.kind == TypeKind::tpError)
    {
        std::stringstream errorStream;

        errorStream << "An error has occurred at the line " << binary->right->location.row << " and the column " << binary->right->location.col << ".\nThe type \"" << type1 << "\" is not compatible with the type \"" << type2 << "\".";
        errors.emplace_back(errorStream.str());

        result = false;
    }
    
    return result;
}

#pragma region helpers
Type SemanticAnalyser::checkTypes(Type type1, Type type2)
{
    if(type1 == type2)
    {
        return type1;
    }

    return Type{.kind = TypeKind::tpError};
}

Type SemanticAnalyser::checkTypes(Type type, std::initializer_list<Type> others)
{
    for(Type other: others)
    {
        if(type == other)
        {
            return type;
        }
    }

    return Type{.kind = TypeKind::tpError};
}
#pragma endregion