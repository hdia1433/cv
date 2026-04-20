#include "controller.hpp"

Controller::Controller(const std::string& code):code(code)
{

}

void Controller::start()
{
    std::println("Lexing:");
    lexer.analyze(std::move(code));
    lexer.print();

    std::println("\n\nParsing:");
    parser.parse(lexer.getTokens());
    parser.print();

    std::println("\n\nSemantic analysing:");
    sAnalyser.analyse(parser.getAst());
    std::println("No errors found.");

    std::println("\n\nGenerating intermediate code:");
    iRGenerator.generate(parser.getAst());
    iRGenerator.printToFile();
}