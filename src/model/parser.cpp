#include "parser.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), index(0)
{
}

std::vector<std::unique_ptr<nodes::Node>> Parser::parse()
{
    std::vector<std::unique_ptr<nodes::Node>> nodes = parseGlobal();

    for(const std::unique_ptr<nodes::Node>& node : nodes)
    {
        if(node->type == NodeType::function && static_cast<nodes::FunctionDecl*>(node.get())->name == "main")
        {
            return nodes;
        }
    }

    std::cerr << "Couldn't find starting point. must declare void function named 'name'.";
    exit(EXIT_FAILURE);
}

#pragma region structures
std::unique_ptr<nodes::Node> Parser::parseFunc(std::string type, std::string name)
{
    consume();

    if(!peek().has_value() || peek().value().getType() != TokenType::rParen)
    {
        std::cerr << "Expected ')'";
        exit(EXIT_FAILURE);
    }

    consume();

    if(!peek().has_value() || peek().value().getType() != TokenType::lBrace)
    {
        std::cerr << "Expected '{'";
        exit(EXIT_FAILURE);
    }

    consume();

    std::unique_ptr<nodes::FunctionDecl> function = std::make_unique<nodes::FunctionDecl>(type, name, parseBlock());

    return function;
}
#pragma endregion

#pragma region keyWords
std::unique_ptr<nodes::Node> Parser::parseExit()
{
    if(!peek().has_value() || peek().value().getType() != TokenType::exitKey)
    {
        std::cerr << "\nExpected keyword 'Exit'";
        exit(EXIT_FAILURE);
    }

    consume();
    return std::make_unique<nodes::Exit>(parseExpr());
}
#pragma endregion

#pragma region statementTree
std::unique_ptr<nodes::Node> Parser::parseStatement()
{
    if (!peek().has_value())
    {
        std::cerr << "\nExpected statement, none given.";
    }

    std::unique_ptr<nodes::Node> statement;

    switch (peek().value().getType())
    {
    case TokenType::exitKey:
        statement = parseExit();
        break;
    case TokenType::semi:
        statement = std::make_unique<nodes::Empty>();
        consume();
        return statement;
    default:
        statement = parseExpr();
        break;
    }

    if (!peek().has_value() || peek().value().getType() != TokenType::semi)
    {
        std::cerr << "\nExpected ';', none given.";
        exit(EXIT_FAILURE);
    }

    consume();
    return statement;
}

std::unique_ptr<nodes::Node> Parser::parseExpr()
{
    return parsePrimary();
}

std::unique_ptr<nodes::Node> Parser::parsePrimary()
{
    if (peek().has_value() && peek().value().getType() == TokenType::intLit)
    {
        return std::make_unique<nodes::Int>(std::stoi(consume().getValue().value()));
    }

    std::cerr << "\nExpected expression, none given.";
    exit(EXIT_FAILURE);
}

#pragma endregion

#pragma region codeBlocks
std::vector<std::unique_ptr<nodes::Node>> Parser::parseGlobal()
{
    std::vector<std::unique_ptr<nodes::Node>> nodes;

    while(peek().has_value())
    {
        if(!isStdType(peek().value().getType()) && (peek().value().getType() != TokenType::identifier || !isDefType(peek().value().getValue().value())))
        {
            std::cerr << "Expected a declaration.";
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
            std::cerr << "\nExpected identifier.";
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

    return nodes;
}

std::vector<std::unique_ptr<nodes::Node>> Parser::parseBlock()
{
    std::vector<std::unique_ptr<nodes::Node>> funcBody;

    while (peek().has_value())
    {
        if (peek().value().getType() == TokenType::rBrace)
        {
            consume();
            return funcBody;
        }

        funcBody.emplace_back(parseStatement());
    }

    std::cerr << "\nExpected '}'";
    exit(EXIT_FAILURE);
}

#pragma endregion

#pragma region helper
bool Parser::isStdType(TokenType type)
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

std::string Parser::stdTypeToStr(TokenType token)
{
    switch (token)
    {
    case TokenType::intType:
        return "int";
    case TokenType::voidType:
        return "void";
    default:
        std::cerr << "\nExpected type.";
        exit(EXIT_FAILURE);
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

#pragma endregion