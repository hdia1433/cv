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
}

void Parser::parse(const std::vector<Token>& tokens)
{
    this->tokens = tokens;

    std::vector<nodes::Node*> globalNodes = parseGlobal();

    ast.reserve(ast.capacity() + globalNodes.size());

    std::move(globalNodes.begin(), globalNodes.end(), std::back_inserter(ast));
}

void Parser::print()
{

}

#pragma region structures
nodes::Node* Parser::parseFuncDecl(TokenType type, std::string_view name)
{
    nodes::FuncDecl* function;

    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA ')' was expected but the end of the file was found instead. In order to close off the parameter list a ')' is required before the function body.\n";

        nodes::Error error{.error = errorStream.str()};
        errors.emplace_back(std::move(error));
        return &errors.back();
    }

    auto tok = *pTok;
    if(tok.type != TokenType::rParen)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA ')' was expected to end the parameter list, but '" << tok.buffer << "' was found instead. In order to close off the parameter list a ')' is required before the function body.\n";

        nodes::Error error{.error = errorStream.str()};
        errors.emplace_back(std::move(error));
        return &errors.back();
    }

    consume();

    pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA '{' was expected but the end of the file was found instead. In order to begin the body of the function you need a '{'.\n";

        nodes::Error error{.error = errorStream.str()};
        errors.emplace_back(std::move(error));
        return &errors.back();
    }

    tok = *pTok;
    if(tok.type != TokenType::lBrace)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA '{' was expected, but '" << tok.buffer << "' was given instead. In order to begin the body of a function, a '{' is needed.\n";

        nodes::Error error{.error = errorStream.str()};
        errors.emplace_back(std::move(error));
        return &errors.back();
    }

    function = new nodes::FuncDecl(parseBody());

    return function;
}
#pragma endregion

#pragma region bodies
std::vector<nodes::Node*> Parser::parseGlobal()
{
    std::vector<nodes::Node*> nodes;

    while(auto pTok = peek())
    {
        auto tok = *peek();
        if(!isType(tok.type))
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA type was expected but '" << tok.buffer << "' was given instead. In global space, only function declarations and global variable declarations are allowed.\n";

            nodes::Error error{.error = errorStream.str()};
            errors.emplace_back(error);
            nodes.emplace_back(&errors.back());
            continue;
        }

        TokenType type = consume().type;

        pTok = peek();
        if(!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nA name for a function or variable was expected but the end of the file was found instead. All variables must include a name and a semi colon; while all functions must include a name, a parameter list [eg. '(int list, bool of, char parameters)'], and a function body surrounded by '{}'.\n";

            nodes::Error error{.error = errorStream.str()};
            errors.emplace_back(error);
            nodes.emplace_back(&errors.back());
            continue;
        }

        tok = *pTok;
        if(tok.type != TokenType::identifier)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA name for a variable or function was expected but '" << tok.buffer << "' was given instead. All variables must include a name and a semi colon; while all functions must include a name, a parameter list [eg. '(int list, bool of, char parameters)'], and a function body surrounded by '{}'.\n\n";

            nodes::Error error{.error = errorStream.str()};
            errors.emplace_back(error);
            nodes.emplace_back(&errors.back());
            continue;
        }

        std::string_view name = consume().buffer;

        pTok = peek();
        if(!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nAn assignment and a semi colon or a semi colon was expected to end a variable declaration, or a parameter list [eg. '(int list, bool of, char parameters)'] and function body were expected to end the function declaration. However, the end of the file was found instead.\n";

            nodes::Error error{.error = errorStream.str()};
            errors.emplace_back(error);
            nodes.emplace_back(&errors.back());
            continue;
        }

        tok = *pTok;
        if(tok.type == TokenType::lParen)
        {
            nodes.emplace_back(parseFuncDecl(type, name));
        }
    }

    return nodes;
}
#pragma endregion

#pragma region helper
Token* Parser::peek(uint ahead)
{
    return &tokens[index + ahead];
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