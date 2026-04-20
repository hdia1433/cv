#include "pch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semanticAnalyser.hpp"
#include "iRGenerator.hpp"
#include "optimizer.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
    Parser parser;
    SemanticAnalyser sAnalyser;
    IRGenerator iRGenerator;
    Optimizer optimizer;
public:
    Controller(const std::string& code);

    void start();
};