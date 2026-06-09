#include "controller.hpp"
#include "fstream"
#include <unistd.h>

Controller::Controller(const std::string& code, bool optimise): code(code), optimise(optimise)
{
}

void Controller::start()
{
#ifdef DEBUG
    std::println("Lexing.");
#endif
    lexer.analyze(std::move(code));
#ifdef DEBUG
    std::println("Printing to tokens.txt");
    lexer.printToFile();
#endif

#ifdef DEBUG
    std::println("\nParsing.");
#endif
    parser.parse(lexer.getTokens());
#ifdef DEBUG
    std::println("Printing to ast.txt");
    parser.printToFile();
#endif

#ifdef DEBUG
    std::println("\nSemantic analysing.");
#endif
    sAnalyser.analyse(parser.getAst());

#ifdef DEBUG
    std::println("\nGenerating intermediate code.");
#endif
    iRGenerator.generate(parser.getAst());
#ifdef DEBUG
    iRGenerator.printToFile();
#endif

    if (optimise)
    {
#ifdef DEBUG
        std::println("\nOptimizing intermediate code.");
#endif
        optimizer.optimize(iRGenerator.getInstructions());
#ifdef DEBUG
        iRGenerator.printToFile("irgo.cvirg");
#endif
    }

    auto ir = iRGenerator.getInstructions();

#ifdef DEBUG
    std::println("\nGenerating assembly.");
#endif
    asmGenerator.generate(ir);

#ifdef DEBUG
    std::println("\nOutputting assembly to a file.");
#endif
    std::ofstream assemblyFile("out.asm");
    if (assemblyFile.is_open())
    {
        assemblyFile << asmGenerator.getAssembly();

        assemblyFile.close();
    }

#ifdef DEBUG
    std::println("Compiling assembly");
#endif
    system("clang out.asm -o out");
#ifndef DEBUG
    system("rm out.asm");
#endif
}
