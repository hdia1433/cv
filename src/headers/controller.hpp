#include "pch.hpp"
#include "lexer.hpp"

class Controller
{
private:
    std::string code;

    Lexer lexer;
public:
    Controller(const std::string& code);

    void start();
};