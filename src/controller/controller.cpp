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
    parser.parse(std::move(lexer.getTokens()));
    parser.print();
}