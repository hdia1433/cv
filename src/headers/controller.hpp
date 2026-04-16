#include "pch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semanticAnalyser.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
    Parser parser;
    SemanticAnalyser sAnalyser;
public:
    Controller(const std::string& code);

    void start();
};