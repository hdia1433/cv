#include "parser.hpp"
#include <sstream>
#include <fstream>

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
        if(errorNum > 1)
        {
            errorStream << errorNum << " errors were generated.";
        }
        else
        {
            errorStream << "1 error was generated.";
        }
        throw std::runtime_error(errorStream.str());
    }
}

std::vector<nodes::Node*> Parser::getAst()
{
    return ast;
}

void Parser::printToFile()
{
    std::ofstream file("ast.txt");
    if(!file)
    {
        std::println("Couldn't open file 'ast.txt'");
    }
    for(uint i = 0; i < ast.size(); i++)
    {
        if(i == ast.size() - 1)
        {
            file << ast[i]->printToFile(0, 0, true);
            break;
        }
        file << ast[i]->printToFile();
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
    function = new nodes::FuncDecl(parseScope(), name, tokenTypeToPrimitive(type), location);

    return function;
}

nodes::Node* Parser::parseVarDecl(TokenType type, std::string_view name, const Coordinate& location)
{
    Token* pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nAn '=' for variable assignment or ';' for default declaration was expected, but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        return errors.back();
    }

    return new nodes::VarDecl(name, tokenTypeToPrimitive(type), location);
}

nodes::Node* Parser::parseVarRef(std::string_view name, const Coordinate& location)
{
    return new nodes::VarRef(name, location);
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
            if(type == TokenType::kwVoid)
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << location.row << " and the column " << location.col << ".\nA variable cannot be declared as type \"void\".\n";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                nodes.emplace_back(errors.back());
                consume();
                continue;
            }

            nodes.emplace_back(parseVarDecl(type, name, location));
        }
    }

    return nodes;
}

std::vector<nodes::Node*> Parser::parseScope()
{
    std::vector<nodes::Node*> statements;

    while(auto pTok = peek())
    {
        auto tok = *pTok;

        if(tok.type == TokenType::rBrace)
        {
            consume();
            return statements;
        }

        nodes::Node* node = parseStatement();
        if(node->type == NodeType::empty)
        {
            continue;
        }

        statements.emplace_back(node);
    }

    std::stringstream errorStream;
    errorStream << "An error has occurred at the end of the file. A '}' was expected but the end of the file was found instead. A '}' is needed to end a scope or body.\n";
    nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
    errors.emplace_back(std::move(error));
    statements.emplace_back(errors.back());
    consume();

    return statements;
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
            statement = new nodes::Empty;
            break;
        case TokenType::kwVoid:
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA variable cannot be of the type 'void'.";
            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            statement = errors.back();
            break;
        }
        default:
            statement = parseExpression();
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
    return parseAssign();
}

nodes::Node* Parser::parseAssign()
{
    auto pTok = peek();
    if(!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nAn expression was expected, but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        return errors.back();
    }

    nodes::Node* left;

    Token tok = *pTok;
    if(isType(tok.type))
    {
        if(tok.type == TokenType::kwVoid)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line" << tok.location.row << " and the column " << tok.location.col << ".\nA variable cannot be of the type \"void\"";
            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            return errors.back();
        }

        Coordinate location = tok.location;
        TokenType type = consume().type;

        pTok = peek();
        if(!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line end of the file.\nA name for a variable was expected, but the end of the file was found instead. A variable's declaration must contain a name.";
            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            return errors.back();
        }

        tok = *pTok;
        if(tok.type != TokenType::identifier)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nA variable name was expected, but '" << tok.buffer << "' was found instead. A variable's declaration must contain a name.";
            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            return errors.back();
        }

        std::string_view name = consume().buffer;
        left = parseVarDecl(type, name, location);
        pTok = peek();
    }
    else if(tok.type == TokenType::identifier)
    {
        left = parseVarRef(tok.buffer, tok.location);
        pTok = peek(1);
        if(pTok && pTok->type == TokenType::opAssign)
        {
            consume();
            consume();
            nodes::Node* right = parseExpression();
            left = new nodes::Binary("=", left, right, left->location);
            return left;
        }
    }
    else
    {
        return parseTerm();
    }

    if(pTok && pTok->type == TokenType::opAssign)
    {
        consume();
        nodes::Node* right = parseExpression();
        left = new nodes::Binary("=", left, right, left->location);
        return left;
    }

    return parseTerm();
}

nodes::Node* Parser::parseTerm()
{
    nodes::Node* left = parsePrimary();

    auto pTok = peek();
    if(pTok && pTok->type == TokenType::opPlus || pTok->type == TokenType::opMinus)
    {
        std::string op = std::string(consume().buffer);

        nodes::Node* right = parseExpression();
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
        return errors.back();
    }

    auto tok = *pTok;
    switch(tok.type)
    {
        case TokenType::ltInt:
            consume();
            return new nodes::Literal(std::stoi(std::string(tok.buffer)), tok.location);
        case TokenType::identifier:
            tok = consume();
            return new nodes::VarRef(tok.buffer, tok.location);
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
        case TokenType::kwInt:
        case TokenType::kwVoid:
            return true;
        default:
            return false;
    }
}

Primitive Parser::tokenTypeToPrimitive(TokenType type)
{
    switch(type)
    {
        case TokenType::ltInt:
        case TokenType::kwInt:
            return Primitive::intTp;
        case TokenType::kwVoid:
            return Primitive::voidTp;
        default:
            return Primitive::custom;
    }
}
#pragma endregion