#include "pch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semanticAnalyser.hpp"
#include "IRGenerator.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
    Parser parser;
    SemanticAnalyser sAnalyser;
    IRGenerator iRGenerator;
public:
    Controller(const std::string& code);

    void start();
};