#include "controller.hpp"

Controller::Controller(const std::string& code):code(code)
{

}

void Controller::start()
{
    lexer.analyze(std::move(code));
    lexer.print();

    parser.parse(std::move(lexer.getTokens()));
    parser.print();
}