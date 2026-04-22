#include "controller.hpp"
#include "fstream"

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

    std::println("\n\nOptimizing intermediate code:");
    optimizer.optimize(iRGenerator.getInstructions());
    iRGenerator.printToFile("irgo.cvirg");

    std::println("\n\nGenerating assembly:");
    asmGenerator.generate(iRGenerator.getInstructions());

    std::println("\nOutputting assembly to a file.");
    std::ofstream assemblyFile("out.asm");
    if(assemblyFile.is_open())
    {
        assemblyFile << asmGenerator.getAssembly();

        assemblyFile.close();
    }

    std::println("Compiling assembly");
    system("clang out.asm -o out");
}