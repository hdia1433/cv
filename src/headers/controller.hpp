#include "pch.hpp"
#include "lexer.hpp"
#include "parser.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
    Parser parser;
public:
    Controller(const std::string& code);

    void start();
};