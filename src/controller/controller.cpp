#include "controller.hpp"
#include "fstream"
#include <unistd.h>

Controller::Controller(const std::string& code):code(code)
{

}

void Controller::start()
{
    write(2, "A\n", 2);
    #ifdef DEBUG
    std::println("Lexing.");
    #endif
    lexer.analyze(std::move(code));
    #ifdef DEBUG
    std::println("Printing to tokens.txt");
    lexer.printToFile();
    #endif
    write(2, "B\n", 2);

    #ifdef DEBUG
    std::println("\nParsing.");
    #endif
    parser.parse(lexer.getTokens());
    #ifdef DEBUG
    std::println("Printing to ast.txt");
    parser.printToFile();
    #endif
    write(2, "C\n", 2);

    #ifdef DEBUG
    std::println("\nSemantic analysing.");
    #endif
    sAnalyser.analyse(parser.getAst());
    write(2, "D\n", 2);

    #ifdef DEBUG
    std::println("\nGenerating intermediate code.");
    #endif
    iRGenerator.generate(parser.getAst());
    #ifdef DEBUG
    iRGenerator.printToFile();
    #endif
    write(2, "E\n", 2);

    // #ifdef DEBUG
    // std::println("\nOptimizing intermediate code.");
    // #endif
    // optimizer.optimize(iRGenerator.getInstructions());
    // #ifdef DEBUG
    // iRGenerator.printToFile("irgo.cvirg");
    // #endif
    write(2, "F\n", 2);

    auto ir = iRGenerator.getInstructions();

    #ifdef DEBUG
    std::println("\nGenerating assembly.");
    #endif
    std::print("Hello!");
    asmGenerator.generate(ir);
    write(2, "G\n", 2);

    #ifdef DEBUG
    std::println("\nOutputting assembly to a file.");
    #endif
    std::ofstream assemblyFile("out.asm");
    if(assemblyFile.is_open())
    {
        assemblyFile << asmGenerator.getAssembly();

        assemblyFile.close();
    }

    std::println("Compiling assembly");
    system("clang out.asm -o out");
    #ifndef DEBUG
    system("rm out.asm");
    #endif
}