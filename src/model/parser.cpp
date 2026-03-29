#include "parser.hpp"
#include "sstream"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), index(0), stackOff(0), row(1), col(1)
{
}

std::vector<std::unique_ptr<nodes::Node>> Parser::parse()
{
    std::vector<std::unique_ptr<nodes::Node>> nodes = parseGlobal();

    for(const std::unique_ptr<nodes::Node>& node : nodes)
    {
        if(node->type == NodeType::functionDecl && static_cast<nodes::FunctionDecl*>(node.get())->name == "main")
        {
            return nodes;
        }
    }
    
    std::stringstream errorStream;

    errorStream << "Couldn't find starting point. must declare void function named 'name'.\n" <<
        "\tvoid main()\n" <<
        "\t{\n" <<
        "\t\t//starting code here.\n" <<
        "\t}";
    
    throw std::runtime_error(errorStream.str());
}

std::vector<std::unique_ptr<nodes::Symbol>> Parser::getAllSymbols()
{
    return std::move(allSymbols);
}

#pragma region structures
std::unique_ptr<nodes::Node> Parser::parseVar(const std::string& type, const std::string& name)
{
    if(isStdType(type))
    {
        align(typeToSize(type));
        if(scopeStack.back().symbols.find(name) == scopeStack.back().symbols.end())
        {
            allSymbols.emplace_back(std::make_unique<nodes::Symbol>(type, stackOff, typeToSize(type)));
            scopeStack.back().symbols[name] = allSymbols.back().get();
        }

        
        const Token* tok = peek();
        if(!tok)
        {
            throw std::runtime_error("\nAn error has occurred.\n A '=' or ';' was expected after the declaration of a variable, but the end of the file was found instead.");
        }

        if(tok->getType() == TokenType::semi)
        {
            int offset = stackOff;

            stackOff += typeToSize(type);

            return std::make_unique<nodes::VarDecl>(type, name, stdTypeToValue(type), offset, typeToSize(type));
        }
        else if(tok->getType() != TokenType::assign)
        {
            updateRowCol(*tok);
            throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA '=' or ';' was expected to finish the declaration of a variable, but '" + tok->getLexeme() + "' was found instead.");
        }

        consume();

        const Token* next = peek();
        if(!next)
        {
            throw std::runtime_error("\nAn error has occurred.\nA value was expected to follow '=' when assigning a value to a variable, but the end of the file was found instead.");
        }

        if(!isStdLit(next->getType()) && next->getType() != TokenType::identifier)
        {
            updateRowCol(*tok);
            throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA value was expected to follow the '=' when assigning a value to a variable, but '" + tok->getLexeme() + "' was found instead.");
        }

        return std::make_unique<nodes::VarDecl>(type, name, parseExpr(), stackOff, typeToSize(type));
    }

    throw std::runtime_error("\nCustom types not yet implemented");
}

std::unique_ptr<nodes::Node> Parser::parseFunc(const std::string& type, const std::string& name)
{
    consume();

    const Token* tok = peek();
    if(!tok)
    {
        throw std::runtime_error("An error has occurred.\n')' expected, but the end of the file was found instead.");
    }

    if(tok->getType() != TokenType::rParen)
    {
        updateRowCol(*tok);
        throw std::runtime_error("\nAn error occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA ')' was expected to close the parameter list.");
    }

    consume();

    const Token* next = peek();
    if(!next)
    {
        throw std::runtime_error("An error has occurred. A '{' was expected after the declaration of a function, but the end of the file was found.");
    }

    if(next->getType() != TokenType::lBrace)
    {
        updateRowCol(*next);
        throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ". A '{' was expected after the declaration of a function, but '" + next->getLexeme() + "' was found instead.");
    }

    consume();

    std::unique_ptr<nodes::FunctionDecl> function = std::make_unique<nodes::FunctionDecl>(type, name, parseBlock());

    return function;
}
#pragma endregion

#pragma region keyWords
std::unique_ptr<nodes::Node> Parser::parseAbort()
{
    consume();
    return std::make_unique<nodes::Abort>(parseExpr());
}
#pragma endregion

#pragma region statementTree
std::unique_ptr<nodes::Node> Parser::parseStatement()
{
    std::unique_ptr<nodes::Node> statement;

    const Token* tok = peek();
    if(isStdType(tok->getType()) || (tok->getType() == TokenType::identifier && isDefType(tok->getLexeme())))
    {
        std::string type;

        if(tok->getType() == TokenType::kwVoid)
        {
            updateRowCol(*tok);
            throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA variable cannot be of the type void. Please replace 'void' with another type.");
        }
        
        const Token* nameTok = peek(1);
        if(!nameTok)
        {
            throw std::runtime_error("\nAn error has occurred. A variable name was expected after it's type, but the end of the file was found.");
        }

        if(nameTok->getType() != TokenType::identifier)
        {
            updateRowCol(*nameTok);
            throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nThe name of a variable was expected to come after it's type, but '" + nameTok->getLexeme() + "' was found instead");
        }

        std::string type = consume().getLexeme();
        std::string name = consume().getLexeme();
        statement = parseVar(type, name);
    }
    else
    {
        switch (tok->getType())
        {
        case TokenType::kwAbort:
            statement = parseAbort();
            break;
        case TokenType::semi:
            statement = std::make_unique<nodes::Empty>();
            consume();
            return statement;
        default:
            statement = parseExpr();
            break;
        }
    }

    const Token* next = peek();
    if(!next)
    {
        throw std::runtime_error("\nAn error has occurred.\nA ';' was expected to end the statement, but the end of the file was found instead.");
    }
    if (next->getType() != TokenType::semi)
    {
        updateRowCol(*next);
        throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA ';' was expected to end the statement, but '" + next->getLexeme() + "' was found instead.");
    }

    consume();
    return statement;
}

std::unique_ptr<nodes::Node> Parser::parseExpr(const std::string& desiredType)
{
    return parsePrimary();
}

std::unique_ptr<nodes::Node> Parser::parsePrimary(const std::string& desiredType)
{
    const Token* tok = peek();
    if(!tok)
    {
        throw std::runtime_error("\nAn error has occurred.\nA variable or literal was expected, but the end of the file was found instead.");
    }
    if (isStdLit(tok->getType()))
    {
        return stdTypeToValue(tok->getType(), consume().getValue().value());
    }
    else if (tok->getType() != TokenType::identifier)
    {
        updateRowCol(*tok);
        throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA variable or literal was expected, but '" + tok->getLexeme() + "' was found instead.");
    }

    auto symbol = findVarInScope(tok);
    if(!symbol)
    {
        updateRowCol(*tok);
        throw std::runtime_error("\nAn error at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\n'" + tok->getLexeme() + "' is not a defined variable or keyword. Please correct the name or define it before it's use.");
    }

    if(!desiredType.empty() && symbol->type != desiredType)
    {
        updateRowCol(*tok);
        throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nThe variable \"" + tok->getLexeme() + "\" is of the type \"" + symbol->type + "\". However the desired type is \"" + desiredType + "\" and doesn't match.");
    }

    consume();
    return std::make_unique<nodes::VarRef>(tok->getLexeme(), symbol);
}

#pragma endregion

#pragma region codeBlocks
std::vector<std::unique_ptr<nodes::Node>> Parser::parseGlobal()
{
    std::vector<std::unique_ptr<nodes::Node>> nodes;

    scopeStack.emplace_back();

    while(auto tok = peek())
    {
        if(!isStdType(tok->getType()) && (tok->getType() != TokenType::identifier || !isDefType(tok->getLexeme())))
        {
            updateRowCol(*tok);
            throw std::runtime_error("\nAn error was found at line " + std::to_string(row) + " and column " + std::to_string(col) + ".\nA type was expected to begin the declaration of a variable or a function. However '" + tok->getLexeme() + "' was found instead.");
        }

        std::string type = "";

        if(isStdType(tok->getType()))
        {
            type = stdTypeToStr(consume().getType());
        }
        else
        {
            type = consume().getLexeme();
        }

        auto next = peek();
        if(!next)
        {
            throw std::runtime_error("\nAn error has occurred. \nA name for a variable or function was expected, but the end of file was found.");
        }
        if(next->getType() != TokenType::identifier)
        {
            updateRowCol(*next);
            throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + "\nExpected a variable or function name after a type. '" + next->getLexeme() + "' was found instead.");
        }

        std::string name = consume().getLexeme();

        auto tokValue = peek();
        if(!tokValue)
        {
            throw std::runtime_error("An error has occurred. Expected '(' for a function declaration or '=' or ';' for global variable declaration. The end of the file was found instead.");
        }
        if(tokValue->getType() == TokenType::lParen)
        {
            nodes.emplace_back(parseFunc(std::move(type), std::move(name)));
        }
        else
        {
            nodes.emplace_back(parseVar(std::move(type), std::move(name)));
        }
    }

    scopeStack.pop_back();

    return nodes;
}

std::vector<std::unique_ptr<nodes::Node>> Parser::parseBlock()
{
    std::vector<std::unique_ptr<nodes::Node>> blockBody;

    int beginStackOff = stackOff;

    scopeStack.emplace_back();

    while (auto tok = peek())
    {
        if (tok->getType() == TokenType::rBrace)
        {
            consume();
            stackOff = beginStackOff;

            scopeStack.pop_back();
            return blockBody;
        }

        std::unique_ptr<nodes::Node> statement = parseStatement();
        
        if(statement->type == NodeType::empty)
        {
            continue;
        }
        blockBody.emplace_back(std::move(statement));
    }

    throw std::runtime_error("\nAn error has occurred.\nA '}' was expected to end the scope, but the end of the file was found instead.");
}

#pragma endregion

#pragma region helper
bool Parser::isStdType(const TokenType& type)
{
    switch (type)
    {
    case TokenType::kwInt:
    case TokenType::kwVoid:
        return true;
    default:
        return false;
    }
}

bool Parser::isStdType(const std::string& type)
{
    if(type == "int" || type == "void")
    {
        return true;
    }
    return false;
}

bool Parser::isStdLit(const TokenType& type)
{
    switch(type)
    {
        case TokenType::intLit:
            return true;
            break;
        default:
            return false;
    }
}

std::string Parser::stdTypeToStr(TokenType token)
{
    switch (token)
    {
    case TokenType::intLit:
    case TokenType::kwInt:
        return "int";
    case TokenType::kwVoid:
        return "void";
    default:
        throw std::runtime_error("\nExpected type.");
    }
}

std::unique_ptr<nodes::Node> Parser::stdTypeToValue(const TokenType& type, const TokenValue& value)
{
    switch(type)
    {
        case TokenType::intLit:
            return std::make_unique<nodes::Int>(std::get<int>(value));
        default:
            throw std::runtime_error("\nExpected type.");
            break;
    }
}

std::unique_ptr<nodes::Node> Parser::stdTypeToValue(const std::string& type)
{
    if(type == "int")
    {
        return std::make_unique<nodes::Int>();
    }

    throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nThe type \"" + type + "\" is unknown.");
}

bool Parser::isDefType(std::string type)
{
    for (std::string defType : types)
    {
        if (type == defType)
        {
            return true;
        }
    }
    return false;
}

unsigned int Parser::typeToSize(const std::string& type)
{

    if (type == "void")
    {
        throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nA variable cannot be of the type \"void\".");
    }
    else if(isDefType(type))
    {
        throw std::runtime_error("\nCustom types not yet implemented");
    }
    else if(type == "int")
    {
        return 4;
    }

    throw std::runtime_error("\nAn error has occurred at the line " + std::to_string(row) + " and the column " + std::to_string(col) + ".\nThe type \"" + type + "\" is unkown.");
}

void Parser::align(const int& alignment)
{
    stackOff = (stackOff + alignment - 1) & ~(alignment - 1);
}

const Token* Parser::peek(unsigned int ahead)
{
    if (index + ahead >= tokens.size())
    {
        return {};
    }

    return &tokens[index + ahead];
}

const Token& Parser::consume()
{
    return tokens[index++];
}

void Parser::updateRowCol(const Token& token)
{
    row = token.getRow();
    col = token.getCol();
}

nodes::Symbol* Parser::findVarInScope(const Token* token)
{
    for(auto pScope = scopeStack.rbegin(); pScope != scopeStack.rend(); pScope++)
    {
        auto symbol = pScope->symbols.find(token->getLexeme());
        if(symbol != pScope->symbols.end())
        {
            return symbol->second;
        }
    }

    return nullptr;
}
#pragma endregion