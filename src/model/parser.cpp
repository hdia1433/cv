#include "parser.hpp"
#include "helpers.hpp"
#include "node.hpp"
#include "token.hpp"
#include "typeKind.hpp"
#include <fstream>
#include <sstream>
#include <string>

Parser::Parser(): index(0)
{
}

Parser::~Parser()
{
    for (nodes::Node* node : ast)
    {
        if (node)
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

    if (errors.size() > 0)
    {
        uint errorNum = 0;

        for (nodes::Error* error : errors)
        {
            errorNum++;
            std::cerr << error->error << std::endl << std::endl;
        }

        std::stringstream errorStream;
        if (errorNum > 1)
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
    if (!file)
    {
        std::println("Couldn't open file 'ast.txt'");
    }
    for (uint i = 0; i < ast.size(); i++)
    {
        if (i == ast.size() - 1)
        {
            file << ast[i]->printToFile(0, 0, true);
            break;
        }
        file << ast[i]->printToFile();
    }
}

#pragma region structures
nodes::Node* Parser::parseFuncDecl(Type type, std::string_view name, const Coordinate& location)
{
    nodes::FuncDecl* function;

    auto pTok = peek();
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA ')' was "
                       "expected but the end of the file was found "
                       "instead. In order to close off the parameter list a ')' is "
                       "required before the function body.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    auto tok = *pTok;
    if (tok.type != TokenType::rParen)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                    << tok.location.col << ".\nA ')' was expected to end the parameter list, but '" << tok.buffer
                    << "' was found instead. In order to close off the parameter "
                       "list a ')' is required before the "
                       "function body.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    consume();

    pTok = peek();
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA '{' was "
                       "expected but the end of the file was "
                       "found instead. In order to begin the body of the function "
                       "you need a '{'.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    tok = *pTok;
    if (tok.type != TokenType::lBrace)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                    << tok.location.col << ".\nA '{' was expected, but '" << tok.buffer
                    << "' was given instead. In order to begin the body of a "
                       "function, a '{' is needed.\n";

        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    consume();
    function = new nodes::FuncDecl(parseScope(), name, std::move(type), location);

    return function;
}

nodes::Node* Parser::parseVarDecl(Type type, std::string_view name, const Coordinate& location)
{
    Token* pTok = peek();
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nAn '=' for "
                       "variable assignment or ';' for "
                       "default declaration was expected, but the end of the file "
                       "was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        return errors.back();
    }

    return new nodes::VarDecl(name, std::move(type), location);
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

    while (auto pTok = peek())
    {
        int peekLevel = 0;

        auto tok = *pTok;
        if (tok.type == TokenType::semi)
        {
            consume();
            continue;
        }

        Coordinate location = tok.location;
        if (!isType(tok.type))
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                        << tok.location.col << ".\nA type was expected but '" << tok.buffer
                        << "' was given instead. In global space, only function "
                           "declarations and global variable "
                           "declarations are allowed.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        Type type = tokenTypeToVarType(tok.type);

        pTok = peek(++peekLevel);
        if (!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nA name "
                           "for a function or variable was "
                           "expected but the end of the file was found instead. All "
                           "variables must include a name and "
                           "a semi colon; while all functions must include a name, a "
                           "parameter list [eg. '(int list, "
                           "bool of, char parameters)'], and a function body "
                           "surrounded by '{}'.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        tok = *pTok;
        bool hasModifiers = false;
        while (tok.type == TokenType::lBracket || tok.type == TokenType::opStar)
        {
            hasModifiers = true;
            if (TokenType::opStar == tok.type)
            {
                consume();
                type = Type(TypeKind::tpPoint, std::make_unique<Type>(type));
                pTok = peek();
                if (!pTok) { break; }
                tok = *pTok;
                continue;
            }
            consume();
            pTok = peek();
            if (!pTok)
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the end of the file.\nA "
                               "size for the array or ']' was expected to "
                               "end the array type declaration. Arrays are declared "
                               "like <type>[<size of array>], or if "
                               "initialised with an initialiser list you can forgo "
                               "the size declaration.\n";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                nodes.emplace_back(errors.back());
                consume();
                continue;
            }
            tok = *pTok;
            int size = 0;
            if (tok.type == TokenType::ltInt)
            {
                consume();
                size = std::stoi(std::string(tok.buffer));
                pTok = peek();
                if (!pTok)
                {
                    std::stringstream errorStream;
                    errorStream << "An error has occurred at the end of the file.\nA ']' was expected to close the array size declaration.\n";
                    nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                    errors.emplace_back(error);
                    nodes.emplace_back(errors.back());
                    consume();
                    continue;
                }
                tok = *pTok;
            }
            if (tok.type == TokenType::rBracket)
            {
                consume();
                type = Type(TypeKind::tpArray, std::make_unique<Type>(type), size);
            }
            else
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                            << tok.location.col << ".\nA number or a ']' was expected but '" << tok.buffer
                            << "' was found instead. Arrays are declared like "
                               "<type>[<size of array>], or if "
                               "initialised with an initialiser list you can forgo "
                               "the size declaration.\n";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                nodes.emplace_back(errors.back());
                consume();
            }
            pTok = peek();
            if (!pTok) { break; }
            tok = *pTok;
        }

        if (hasModifiers)
        {
            peekLevel = 0;
        }

        if (tok.type != TokenType::identifier)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                        << tok.location.col << ".\nA name for a variable or function was expected but '" << tok.buffer
                        << "' was given instead. All variables must include a name and a "
                           "semi colon; while all "
                           "functions must include a name, a parameter list [eg. '(int list, "
                           "bool of, char "
                           "parameters)'], and a function body surrounded by '{}'.\n\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        std::string_view name = tok.buffer;

        pTok = peek(++peekLevel);
        if (!pTok)
        {
            std::stringstream errorStream;
            errorStream << "An error has occurred at the end of the file.\nAn assignment and "
                           "a semi colon or a semi "
                           "colon was expected to end a variable declaration, or a parameter "
                           "list [eg. '(int list, "
                           "bool of, char parameters)'] and function body were expected to "
                           "end the function "
                           "declaration. However, the end of the file was found instead.\n";

            nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
            errors.emplace_back(error);
            nodes.emplace_back(errors.back());
            consume();
            continue;
        }

        tok = *pTok;
        if (tok.type == TokenType::lParen)
        {
            consume();
            consume();
            consume();
            nodes.emplace_back(parseFuncDecl(std::move(type), name, location));
        }
        else
        {
            if (type.kind == TypeKind::tpVoid)
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << location.row << " and the column "
                            << location.col << ".\nA variable cannot be declared as type \"void\".\n";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                nodes.emplace_back(errors.back());
                consume();
                continue;
            }

            nodes.emplace_back(parseAssign());

            if (!(pTok = peek()))
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the end of the file. A ';' "
                               "was expected but the end of the "
                               "file was found instead. A ';' is needed to end every "
                               "statement.";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                consume();
                continue;
            }

            tok = *pTok;
            if (tok.type != TokenType::semi)
            {
                std::stringstream errorStream;
                errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                            << tok.location.col << ".\nA ';' was expected but '" << tok.buffer
                            << "' was found instead. A semicolon is needed to end a statement.";
                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                consume();
                continue;
            }
            consume();
        }
    }

    return nodes;
}

std::vector<nodes::Node*> Parser::parseScope()
{
    std::vector<nodes::Node*> statements;

    while (auto pTok = peek())
    {
        auto tok = *pTok;
        if (tok.type == TokenType::rBrace)
        {
            consume();
            return statements;
        }

        nodes::Node* node = parseStatement();
        if (node->type == NodeType::empty)
        {
            continue;
        }

        statements.emplace_back(std::move(node));
    }

    std::stringstream errorStream;
    errorStream << "An error has occurred at the end of the file. A '}' was "
                   "expected but the end of the file was found "
                   "instead. A '}' is needed to end a scope or body.\n";
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
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. An "
                       "expression was expected but the end of the "
                       "file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    return new nodes::Abort(std::move(parseExpression()), location);
}
#pragma endregion

#pragma region statement tree
nodes::Node* Parser::parseStatement()
{
    auto pTok = peek();
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. A statement "
                       "was expected, but the end of the "
                       "file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    auto tok = *pTok;

    nodes::Node* statement;

    switch (tok.type)
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
                errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                            << tok.location.col << ".\nA variable cannot be of the type 'void'.";
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
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file. A ';' was "
                       "expected but the end of the file was "
                       "found instead. A ';' is needed to end every statement.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        consume();
        return errors.back();
    }

    tok = *pTok;
    if (tok.type != TokenType::semi)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the line " << tok.location.row << " and the column "
                    << tok.location.col << ".\nA ';' was expected but '" << tok.buffer
                    << "' was found instead. A semicolon is needed to end a statement.";
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
    nodes::Node* left = parseUnary();

    auto pTok = peek();
    if(pTok && TokenType::opAssign == pTok->type)
    {
        consume();
        nodes::Node* right = parseExpression();
        left = new nodes::Binary("=", left, right, left->location);
    }

    return left;
}

nodes::Node* Parser::parseTerm()
{
    nodes::Node* left = parseFactor();

    auto pTok = peek();
    if (pTok && (pTok->type == TokenType::opPlus || pTok->type == TokenType::opMinus))
    {
        std::string op = std::string(consume().buffer);

        nodes::Node* right = parseExpression();
        left = new nodes::Binary(op, left, right, left->location);
    }

    return left;
}

nodes::Node* Parser::parseFactor()
{
    nodes::Node* left = parseUnary();

    auto pTok = peek();
    if (pTok && helpers::equalsOr(pTok->type, {TokenType::opStar}))
    {
        std::string op = std::string(consume().buffer);

        nodes::Node* right = parseExpression();
        left = new nodes::Binary(op, left, right, left->location);
    }

    return left;
}

nodes::Node* Parser::parseUnary()
{
    auto pTok = peek();
    if (!pTok)
    {
        nodes::Error* error = new nodes::Error("This error shouldn't print. It's in the \"parseUnary()\" function.");
        errors.emplace_back(error);
        return errors.back();
    }

    auto tok = *pTok;
    switch (tok.type)
    {
        case TokenType::opRef:
            tok = consume();
            return new nodes::Unary("&", parsePostfix(), tok.location);
        case TokenType::opStar:
            tok = consume();
            return new nodes::Unary("*", parsePostfix(), tok.location);
        default:
            return parsePostfix();
    }
}

nodes::Node* Parser::parsePostfix()
{
    nodes::Node* expression = parsePrimary();

    auto pTok = peek();

    if(pTok)
    {
        while (true)
        {
            auto tok = *pTok;
            
            if(TokenType::lBracket == tok.type)
            {
                consume();
                pTok = peek();
                if(!pTok)
                {
                    nodes::Error* error = new nodes::Error("An error has occurred at the end of the file.\nAn index for the array subscript was expected, but the end of the file was found instead.\n");
                    errors.emplace_back(error);
                    return errors.back();
                }

                nodes::Node* index = parsePrimary();

                pTok = peek();
                if (!pTok)
                {
                    nodes::Error* error = new nodes::Error("An error has occurred at the end of the file.\nA ']' was expected to close the subscript, but the end of the file was found instead.\n");
                    errors.emplace_back(error);
                    return errors.back();
                }

                consume();

                expression = new nodes::Subscript(expression, index, expression->location);
            }
            else
            {
                break;
            }

            pTok = peek();
            if(!pTok)
            {
                break;
            }
        }
    }

    return expression;
}

nodes::Node* Parser::parsePrimary()
{
    auto pTok = peek();
    if (!pTok)
    {
        std::stringstream errorStream;
        errorStream << "An error has occurred at the end of the file.\nA literal, "
                       "variable name, or initialiser list "
                       "was expected, but the end of the file was found instead.";
        nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
        errors.emplace_back(error);
        return errors.back();
    }

    auto tok = *pTok;
    switch (tok.type)
    {
        case TokenType::ltInt:
            tok = consume();
            return new nodes::Literal(std::stoi(std::string(tok.buffer)), tok.location);
        case TokenType::kwInt:
        case TokenType::kwVoid:
        case TokenType::kwChar:
        {
            Type type = tokenTypeToVarType(tok.type);
            consume();

            Coordinate location = tok.location;

            pTok = peek();
            if(!pTok)
            {
                nodes::Error* error = new nodes::Error("An error has occurred at the end of the file. Further variable inforamtion (e.g. *, [) or a variable name were expected, but the end of the file was found instead.\n");
                errors.emplace_back(error);
                return errors.back();
            }

            tok = *pTok;
            while(TokenType::identifier != tok.type)
            {
                switch(tok.type)
                {
                    case TokenType::opStar:
                        consume();
                        type = Type(TypeKind::tpPoint, std::make_unique<Type>(type));
                        break;
                    case TokenType::lBracket:
                    {
                        consume();
                        pTok = peek();
                        if(!pTok)
                        {
                            nodes::Error* error = new nodes::Error("An error has occurred at the end of the file. An array size or ']' was expected, but the end of the file was found instead.\n");
                            errors.emplace_back(error);
                            return errors.back();
                        }

                        tok = *pTok;
                        int size = 0;
                        
                        if(TokenType::ltInt == tok.type)
                        {
                            consume();
                            size = std::stoi(std::string(tok.buffer));

                            pTok = peek();
                            if (!pTok)
                            {
                                nodes::Error* error = new nodes::Error("An error has occurred at the end of the file. A ']' was expected to finish the array initialization, but the end of the file was found instead.\n");
                                errors.emplace_back(error);
                                return errors.back();
                            }

                            tok = *pTok;
                        }

                        if(TokenType::rBracket == tok.type)
                        {
                            consume();
                            type = Type(TypeKind::tpArray, std::make_unique<Type>(type), size);
                        }
                        else
                        {
                            std::stringstream errorStream;
                            errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nAn array size or ']' was expected to finish the array type declaration, but '" << tok.buffer << "' was found instead.\n";
                            nodes::Error* error = new nodes::Error(errorStream.str());
                            errors.emplace_back(error);
                            return errors.back();
                        }
                        break;
                    }
                    case TokenType::identifier:
                        break;
                    default:
                    {
                        std::stringstream errorStream;
                        errorStream << "An error has occurred at the line " << tok.location.row << " and the column " << tok.location.col << ".\nFurther variable information (e.g. *, [) or a variable name was expected, but '" << tok.buffer << "' was found instead.\n";
                        nodes::Error* error = new nodes::Error(errorStream.str());
                        errors.emplace_back(error);
                        return errors.back();
                    }
                }

                pTok = peek();
                if(!pTok)
                {
                    nodes::Error* error = new nodes::Error("An error has occurred at the end of the file. Further variable inforamtion (e.g. *, [) or a variable name were expected, but the end of the file was found instead.\n");
                    errors.emplace_back(error);
                    return errors.back();
                }
                tok = *pTok;
            }

            consume();
            std::string_view name = tok.buffer;

            return new nodes::VarDecl(name, type, location);
        }
        case TokenType::identifier:
            {
                tok = consume();
                std::string_view name = tok.buffer;
                pTok = peek();
                return new nodes::VarRef(name, tok.location);
            }
        case TokenType::ltChar:
            tok = consume();
            return new nodes::Literal(tok.buffer[0], tok.location);
        case TokenType::lBrace:
            {
                Coordinate location = consume().location;

                std::vector<nodes::Node*> values;

                while ((pTok = peek()))
                {
                    tok = *pTok;
                    if (tok.type == TokenType::rBrace)
                    {
                        consume();
                        return new nodes::InitList(values, location);
                    }
                    else if (tok.type == TokenType::comma)
                    {
                        consume();
                    }

                    values.emplace_back(parseExpression());
                }

                std::stringstream errorStream;
                errorStream << "An error has occurred at the end of the file.\nA '}' was "
                               "expected to close the initialiser "
                               "list, but the end of the file was found instead.\n";

                nodes::Error* error = new nodes::Error(std::move(errorStream.str()));
                errors.emplace_back(error);
                return errors.back();
            }
        default:
            break;
    }

    return new nodes::Empty;
}
#pragma endregion

#pragma region helper
Token* Parser::peek(uint ahead)
{
    if (index + ahead < tokens.size())
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
    switch (type)
    {
        case TokenType::kwInt:
        case TokenType::kwVoid:
        case TokenType::kwChar:
            return true;
        default:
            return false;
    }
}

Type Parser::tokenTypeToVarType(TokenType type)
{
    switch (type)
    {
        case TokenType::ltInt:
        case TokenType::kwInt:
            return Type(TypeKind::tpInt);
        case TokenType::kwVoid:
            return Type(TypeKind::tpVoid);
        case TokenType::kwChar:
        case TokenType::ltChar:
            return Type(TypeKind::tpChar);
        default:
            return Type(TypeKind::tpError);
    }
}
#pragma endregion
