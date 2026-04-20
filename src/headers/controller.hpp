#include "pch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semanticAnalyser.hpp"
#include "iRGenerator.hpp"
#include "optimizer.hpp"
#include "asmGenerator.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
    Parser parser;
    SemanticAnalyser sAnalyser;
    IRGenerator iRGenerator;
    Optimizer optimizer;
    AsmGenerator asmGenerator;
public:
    Controller(const std::string& code);

    void start();
};