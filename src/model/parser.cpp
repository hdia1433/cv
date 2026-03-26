#include "parser.hpp"

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

    std::cerr << "Couldn't find starting point. must declare void function named 'name'.\n" <<
        "\tvoid main()\n" <<
        "\t{\n" <<
        "\t\t//starting code here.\n" <<
        "\t}";
    exit(EXIT_FAILURE);
}

std::vector<std::unique_ptr<nodes::Symbol>> Parser::getAllSymbols()
{
    return std::move(allSymbols);
}

#pragma region structures
std::unique_ptr<nodes::Node> Parser::parseVar(Token typeToken, const std::string name)
{
    align(typeToSize(typeToken.getType()));
    if(isStdType(typeToken.getType()))
    {
        if(scopeStack.back().symbols.find(name) == scopeStack.back().symbols.end())
        {
            allSymbols.emplace_back(std::make_unique<nodes::Symbol>(stdTypeToStr(typeToken.getType()), stackOff, typeToSize(typeToken.getType())));
            scopeStack.back().symbols[name] = allSymbols.back().get();
        }

        if(peek().has_value() && peek().value().getType() == TokenType::semi)
        {
            return std::make_unique<nodes::VarDecl>(stdTypeToStr(typeToken.getType()), name, stdTypeToValue(typeToken.getType()), stackOff, typeToSize(typeToken.getType()));

            stackOff += typeToSize(typeToken.getType());
        }
        else if(!peek().has_value() || peek().value().getType() != TokenType::assign)
        {
            if(!peek().has_value())
            {
                std::cerr << "\nAn error has occurred.\n A '=' or ';' was expected after the declaration of a variable, but the end of the file was found instead.";
            }
            else
            {
                updateRowCol(peek().value());
                std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nA '=' or ';' was expected to finish the declaration of a variable, but '" << peek().value().getBuffer() << "' was found instead.";
            }
            exit(EXIT_FAILURE);
        }

        consume();

        if(!peek().has_value() || (!isStdLit(peek().value().getType()) && peek().value().getType() != TokenType::identifier))
        {
            if(!peek().has_value())
            {
                std::cerr << "\nAn error has occurred.\nA value was expected to follow '=' when assigning a value to a variable, but the end of the file was found instead.";
            }
            else
            {
                updateRowCol(peek().value());
                std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nA value was expected to follow the '=' when assigning a value to a variable, but '" << peek().value().getBuffer() << "' was found instead.";
            }
            exit(EXIT_FAILURE);
        }

        return std::make_unique<nodes::VarDecl>(stdTypeToStr(typeToken.getType()), name, parseExpr(), stackOff, typeToSize(typeToken.getType()));
    }

    std::cerr << "\nCustom types not yet implemented";
    exit(EXIT_FAILURE);
}

std::unique_ptr<nodes::Node> Parser::parseFunc(std::string type, std::string name)
{
    consume();

    if(!peek().has_value() || peek().value().getType() != TokenType::rParen)
    {
        if(!peek().has_value())
        {
            std::cerr << "An error has occurred.\n')' expected, but the end of the file was found instead.";
        }
        else
        {
            updateRowCol(peek().value());
            std::cerr << "Parameters not implemented yet.";
            std::cerr << "\nAn error occurred at the line " << row << " and the column " << col << ".\nA ')' was expected to close the parameter list.";
        }
        exit(EXIT_FAILURE);
    }

    consume();

    if(!peek().has_value() || peek().value().getType() != TokenType::lBrace)
    {
        if(!peek().has_value())
        {
            std::cerr << "An error has occurred. A '{' was expected after the declaration of a function, but the end of the file was found.";
        }
        else
        {
            updateRowCol(peek().value());
            std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ". A '{' was expected after the declaration of a function, but '" << peek().value().getBuffer() << "' was found instead.";
        }
        exit(EXIT_FAILURE);
    }

    consume();

    std::unique_ptr<nodes::FunctionDecl> function = std::make_unique<nodes::FunctionDecl>(type, name, parseBlock());

    return function;
}
#pragma endregion

#pragma region keyWords
std::unique_ptr<nodes::Node> Parser::parseAbort()
{
    if(!peek().has_value() || peek().value().getType() != TokenType::abortKey)
    {
        std::cerr << "\nThis error should not print. If it prints please contact the developer of the language at procon.loz.botw@gmail.com and mention that the \"Abort keyword expected\" error was printed.";
        exit(EXIT_FAILURE);
    }

    consume();
    return std::make_unique<nodes::Abort>(parseExpr());
}
#pragma endregion

#pragma region statementTree
std::unique_ptr<nodes::Node> Parser::parseStatement()
{
    if (!peek().has_value())
    {
        std::cerr << "\nThis error should never print. If printed contact the creator at procon.loz.botw@gmail.com and notify him that the \"statement expected\" error was printed.";
    }

    std::unique_ptr<nodes::Node> statement;

    if(isStdType(peek().value().getType()) || (peek().value().getType() == TokenType::identifier && isDefType(peek().value().getValue().value())))
    {
        std::string type;

        if(peek().value().getType() == TokenType::voidType)
        {
            updateRowCol(peek().value());
            std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nA variable cannot be of the type void. Please replace 'void' with another type.";
            exit(EXIT_FAILURE);
        }

        if(!peek(1).has_value() || peek(1).value().getType() != TokenType::identifier)
        {
            if(!peek(1).has_value())
            {
                std::cerr << "\nAn error has occurred. A variable name was expected after it's type, but the end of the file was found.";
            }
            else
            {
                updateRowCol(peek(1).value());
                std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nThe name of a variable was expected to come after it's type, but '" << peek(1).value().getBuffer() << "' was found instead";
            }
            exit(EXIT_FAILURE);
        }

        statement = parseVar(std::move(consume()), consume().getValue().value());
    }
    else
    {
        switch (peek().value().getType())
        {
        case TokenType::abortKey:
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

    if (!peek().has_value() || peek().value().getType() != TokenType::semi)
    {
        if(!peek().has_value())
        {
            std::cerr << "\nAn error has occurred.\nA ';' was expected to end the statement, but the end of the file was found instead.";
        }
        else
        {
            updateRowCol(peek().value());
            std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nA ';' was expected to end the statement, but '" << peek().value().getBuffer() << "' was found instead.";
        }
        exit(EXIT_FAILURE);
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
    if (peek().has_value() && isStdLit(peek().value().getType()))
    {
        return stdTypeToValue(peek().value().getType(), consume().getValue().value());
    }
    else if (!peek().has_value() || peek().value().getType() != TokenType::identifier)
    {
        if(!peek().has_value())
        {
            std::cerr << "\nAn error has occurred.\nA variable or literal was expected, but the end of the file was found instead.";
        }
        else
        {
            updateRowCol(peek().value());
            std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nA variable or literal was expected, but '" << peek().value().getBuffer() << "' was found instead.";
        }
        exit(EXIT_FAILURE);
    }

    if(scopeStack.back().symbols.find(peek().value().getValue().value()) == scopeStack.back().symbols.end())
    {
        updateRowCol(peek().value());
        std::cerr << "\nAn error at the line " << row << " and the column " << col << ".\n'" << peek().value().getValue().value() << "' is not a defined variable or keyword. Please correct the name or define it before it's use.";
        exit(EXIT_FAILURE);
    }

    if(!desiredType.empty() && scopeStack.back().symbols.at(peek().value().getValue().value())->type != desiredType)
    {
        updateRowCol(peek().value());
        std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col << ".\nThe variable \"" << peek().value().getValue().value() <<"\" is of the type \"" << scopeStack.back().symbols.at(peek().value().getValue().value())->type << "\". However the desired type is \"" << desiredType << "\" and doesn't match.";
        exit(EXIT_FAILURE);
    }

    return std::make_unique<nodes::VarRef>(peek().value().getValue().value(), scopeStack.back().symbols[consume().getValue().value()]);
}

#pragma endregion

#pragma region codeBlocks
std::vector<std::unique_ptr<nodes::Node>> Parser::parseGlobal()
{
    std::vector<std::unique_ptr<nodes::Node>> nodes;

    scopeStack.emplace_back();

    while(peek().has_value())
    {
        updateRowCol(peek().value());

        if(!isStdType(peek().value().getType()) && (peek().value().getType() != TokenType::identifier || !isDefType(peek().value().getValue().value())))
        {
            std::cerr << "\nAn error was found at line " << row << " and column " << col << ".\nA type was expected to begin the declaration of a variable or a function. However '" << peek().value().getBuffer() << "' was found instead.";
            exit(EXIT_FAILURE);
        }

        std::string type = "";

        if(isStdType(peek().value().getType()))
        {
            type = stdTypeToStr(consume().getType());
        }
        else
        {
            type = consume().getValue().value();
        }

        if(!peek().has_value() || peek().value().getType() != TokenType::identifier)
        {
            if(!peek().has_value())
            {
                std::cerr << "\nAn error has occurred. \nA name for a variable or function was expected, but the end of file was found.";
            }
            else
            {
                updateRowCol(peek().value());
                std::cerr << "\nAn error has occurred at the line " << row << " and the column " << col <<"\nExpected a variable or function name after a type. '" << peek().value().getBuffer() << "' was found instead.";
            }
            exit(EXIT_FAILURE);
        }

        std::string name = consume().getValue().value();

        if(peek().has_value() && peek().value().getType() == TokenType::lParen)
        {
            nodes.emplace_back(parseFunc(std::move(type), std::move(name)));
        }
        else
        {
            std::cerr << "\nExpected '('";
            exit(EXIT_FAILURE);
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

    while (peek().has_value())
    {
        if (peek().value().getType() == TokenType::rBrace)
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

    std::cerr << "\nAn error has occurred.\nA '}' was expected to end the scope, but the end of the file was found instead.";
    exit(EXIT_FAILURE);
}

#pragma endregion

#pragma region helper
bool Parser::isStdType(const TokenType& type)
{
    switch (type)
    {
    case TokenType::intType:
    case TokenType::voidType:
        return true;
    default:
        return false;
    }
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
    case TokenType::intType:
        return "int";
    case TokenType::voidType:
        return "void";
    default:
        std::cerr << "\nExpected type.";
        exit(EXIT_FAILURE);
    }
}

std::unique_ptr<nodes::Node> Parser::stdTypeToValue(const TokenType& type, const std::string& value)
{
    if(value.empty())
    {
        switch(type)
        {
            case TokenType::intLit:
                return std::make_unique<nodes::Int>();
            default:
                std::cerr << "\nExpected literal.";
                exit(EXIT_FAILURE);
                break;
        }
    }

    switch(type)
    {
        case TokenType::intLit:
            return std::make_unique<nodes::Int>(std::stoi(value));
        default:
            std::cerr << "\nExpected type.";
            exit(EXIT_FAILURE);
            break;
    }
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

unsigned int Parser::typeToSize(const TokenType& type)
{
    switch(type)
    {
        case TokenType::voidType:
            std::cerr << "\nVoid type cannot contain a value.";
            exit(EXIT_FAILURE);
        case TokenType::intType:
            return 4;
        case TokenType::identifier:
            std::cerr << "\nCustom types not implemented yet.";
            exit(EXIT_FAILURE);
        default:
            std::cerr << "\nExpected type";
            exit(EXIT_FAILURE);
    }
}

void Parser::align(const int& alignment)
{
    stackOff = (stackOff + alignment - 1) & ~(alignment - 1);
}

std::optional<Token> Parser::peek(unsigned int ahead)
{
    if (index + ahead >= tokens.size())
    {
        return {};
    }

    return tokens[index + ahead];
}

Token Parser::consume()
{
    return tokens[index++];
}

void Parser::updateRowCol(const Token& token)
{
    row = token.getRow();
    col = token.getCol();
}
#pragma endregion