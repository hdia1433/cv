#include "asmGenerator.hpp"
#include "iRGenerator.hpp"
#include "lexer.hpp"
#include "optimizer.hpp"
#include "parser.hpp"
#include "semanticAnalyser.hpp"

class Controller
{
  private:
    std::string code;
    bool optimise;

    Lexer lexer;
    Parser parser;
    SemanticAnalyser sAnalyser;
    IRGenerator iRGenerator;
    Optimizer optimizer;
    AsmGenerator asmGenerator;

  public:
    Controller(const std::string& code, bool optimise = false);

    void start();
};
