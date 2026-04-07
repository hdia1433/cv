#include "parser.hpp"

Parser::Parser():index(0)
{

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

#pragma region bodies
std::vector<nodes::Node*> Parser::parseGlobal()
{
    std::vector<nodes::Node*> nodes;

    

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
#pragma endregion