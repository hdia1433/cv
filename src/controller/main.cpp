#include "pch.hpp"
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "nodes.hpp"
#include "generator.hpp"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cerr << "Error. No filename was provided.\nCorrect usage:\n\tcv <filename>.cv\n";
        return EXIT_FAILURE;
    }
    else if (argc > 2)
    {
        std::cerr << "Error. Only 1 argument expected. Received: " << argc << ".\nCorrect usage:\n\tcv <filename>.cv\n";
        return EXIT_FAILURE;
    }

    std::string code;
    {
        std::ifstream input(argv[1]);
        std::stringstream codeStream;
        codeStream << input.rdbuf();
        input.close();

        code = codeStream.str();
    }

    std::println("\nLexing");

    Lexer lexer(std::move(code));
    std::vector<Token> tokens = lexer.tokenize();

    for(Token token: tokens)
    {
        std::cout << token << " ";
    }

    std::println("\nParsing");

    Parser parser(std::move(tokens));
    auto nodes = parser.parse();

    // std::println("Generating assembly");

    // Generator generator(std::move(nodes));
    
    // std::fstream file("out.asm", std::ios::out);
    // file << generator.generate();
    // file.close();

    // std::println("Building file");

    // system("clang out.asm -o out");

    return 0;
}
