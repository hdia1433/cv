#include "parser.hpp"
#include <sstream>

Parser::Parser():index(0)
{

}

Parser::~Parser()
{
    for(nodes::Node* node: ast)
    {
        if(node)
        {
            delete node;
        }
    }

    ast.clear();

    errors.clear();
}

void Parser::parse(const std::vector<Token>& tokens)
{
    this->tokens = tokens;

    std::vector<nodes::Node*> globalNodes = parseGlobal();

    ast.reserve(ast.capacity() + globalNodes.size());

    std::move(globalNodes.begin(), globalNodes.end(), std::back_inserter(ast));

    if(errors.size() > 0)
    {
        uint errorNum = 0;

        for(nodes::Error* error: errors)
        {
            errorNum++;
            std::cerr << error->error << std::endl << std::endl;
        }

        std::stringstream errorStream;
        errorStream << errorNum << " errors were generated.";
        throw std::runtime_error(errorStream.str());
    }
}

std::vector<nodes::Node*> Parser::getAst()
{
    return ast;
}

void Parser::print()
{
    for(nodes::Node* node: ast)
    {
        node->print();
    }
}

#pragma region structures
nodes::Node* Parser::parseFuncDecl(TokenType type, std::string_view name, const Coordinate& location)
{
    nodes::FuncDecl* function;

    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA ')' was expected but the end of the file was found instead. In order to close off the parameter list a ')' is required before the function body.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    auto tok = *pTok;
    if(tok.type != TokenType::rParen)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA ')' was expected to end the parameter list, but '" << tok.buffer << "' was found instead. In order to close off the parameter list a ')' is required before the function body.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    consume();

    pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA '{' was expected but the end of the file was found instead. In order to begin the body of the function you need a '{'.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    tok = *pTok;
    if(tok.type != TokenType::lBrace)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA '{' was expected, but '" << tok.buffer << "' was given instead. In order to begin the body of a function, a '{' is needed.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    consume();
    function = new nodes::FuncDecl(parseBody(), name, type, location);

    return function;
}
#pragma endregion

#pragma region bodies
std::vector<nodes::Node*> Parser::parseGlobal()
{
    std::vector<nodes::Node*> nodes;

    while(auto pTok = peek())
    {
        auto tok = *pTok;
        if(tok.type == TokenType::semi)
        {
            continue;
        }

        Coordinate location = tok.location;
        if(!isType(tok.type))
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA type was expected but '" << tok.buffer << "' was given instead. In global space, only function declarations and global variable declarations are allowed.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        TokenType type = consume().type;

        pTok = peek();
        if(!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nA name for a function or variable was expected but the end of the file was found instead. All variables must include a name and a semi colon; while all functions must include a name, a parameter list [eg. '(int list, bool of, char parameters)'], and a function body surrounded by '{}'.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        tok = *pTok;
        if(tok.type != TokenType::identifier)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA name for a variable or function was expected but '" << tok.buffer << "' was given instead. All variables must include a name and a semi colon; while all functions must include a name, a parameter list [eg. '(int list, bool of, char parameters)'], and a function body surrounded by '{}'.\n\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        std::string_view name = consume().buffer;

        pTok = peek();
        if(!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nAn assignment and a semi colon or a semi colon was expected to end a variable declaration, or a parameter list [eg. '(int list, bool of, char parameters)'] and function body were expected to end the function declaration. However, the end of the file was found instead.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        tok = *pTok;
        if(tok.type == TokenType::lParen)
        {
            consume();
            nodes.emplace_back(parseFuncDecl(type, name, location));
        }
        else
        {
            std::stringstream errorStream;
            errorStream << "Variables are not implemented yet.\n";
            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(error);
            consume();
        }
    }

    return nodes;
}

std::vector<nodes::Node*> Parser::parseBody()
{
    std::vector<nodes::Node*> body;

    while(auto pTok = peek())
    {
        auto tok = *pTok;

        if(tok.type == TokenType::rBrace)
        {
            consume();
            return body;
        }

        nodes::Node* node = parseStatement();
        if(node->type == NodeType::empty)
        {
            continue;
        }

        body.emplace_back(node);
    }

    std::stringstream errorStream;
    errorStream << "An error has occurred at the end of the file. A '}' was expected but the end of the file was found instead. A '}' is needed to end a scope or body.\n";
    nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
    errors.emplace_back(std::move(error));
    body.emplace_back(errors.back());
    consume();

    return body;
}
#pragma endregion

#pragma region keywords
nodes::Node* Parser::parseAbort(const Coordinate& location)
{
    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. An expression was expected but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    return new nodes::Abort(parseExpression(), location);
}
#pragma endregion

#pragma region statement tree
nodes::Node* Parser::parseStatement()
{
    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. A statement was expected, but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    auto tok = *pTok;

    nodes::Node* statement;

    switch(tok.type)
    {
        case TokenType::kwAbort:
            consume();
            statement = parseAbort(tok.location);
            break;
        case TokenType::semi:
            consume();
            return new nodes::Empty;
        default:
            break;
    }

    pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. A ';' was expected but the end of the file was found instead. A ';' is needed to end every statement.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    tok = *pTok;
    if(tok.type != TokenType::semi)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA ';' was expected but '" << tok.buffer << "' was found instead. A semicolon is needed to end a statement.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    consume();
    return statement;
}

nodes::Node* Parser::parseExpression()
{
    return parseTerm();
}

nodes::Node* Parser::parseTerm()
{
    nodes::Node* left = parsePrimary();

    auto pTok = peek();
    if(pTok && pTok->type == TokenType::opPlus)
    {
        std::string op = std::string(consume().buffer);

        nodes::Node* right = parsePrimary();
        left = new nodes::Binary(op, left, right, left->location);
    }

    return left;
}

nodes::Node* Parser::parsePrimary()
{
    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA number or variable name was expected, but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    auto tok = *pTok;
    switch(tok.type)
    {
        case TokenType::ltInt:
            consume();
            return new nodes::IntLit(std::stoi(std::string(tok.buffer)), tok.location);
        default:
            break;
    }

    return new nodes::Empty;
}
#pragma endregion

#pragma region helper
Token* Parser::peek(uint ahead)
{
    if(index + ahead < tokens.size())
    {
        return &tokens[index + ahead];
    }

    return nullptr;
}

Token Parser::consume()
{
    return tokens[index++];
}

bool Parser::isType(TokenType type)
{
    switch(type)
    {
        case TokenType::kwVoid:
            return true;
        default:
            return false;
    }
}
#pragma endregion