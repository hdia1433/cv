#include "controller.hpp"
#include "fstream"

Controller::Controller(const std::string& code):code(code)
{

}

void Controller::start()
{
    std::println("Lexing.");
    lexer.analyze(std::move(code));
    std::println("Printing to tokens.txt");
    lexer.printToFile();

    std::println("\nParsing.");
    parser.parse(lexer.getTokens());
    std::println("Printing to ast.txt");
    parser.printToFile();

    std::println("\nSemantic analysing.");
    sAnalyser.analyse(parser.getAst());

    std::println("\nGenerating intermediate code.");
    iRGenerator.generate(parser.getAst());
    iRGenerator.printToFile();

    // std::println("\nOptimizing intermediate code.");
    // optimizer.optimize(iRGenerator.getInstructions());
    // iRGenerator.printToFile("irgo.cvirg");

    // std::println("\nGenerating assembly.");
    // asmGenerator.generate(iRGenerator.getInstructions());

    // std::println("\nOutputting assembly to a file.");
    // std::ofstream assemblyFile("out.asm");
    // if(assemblyFile.is_open())
    // {
    //     assemblyFile << asmGenerator.getAssembly();

    //     assemblyFile.close();
    // }

    // std::println("Compiling assembly");
    // system("clang out.asm -o out");
}