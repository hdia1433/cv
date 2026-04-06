#include "pch.hpp"
#include "controller.hpp"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        throw std::runtime_error("An error has occurred. A file was expected to be passed. cv <file>");
    }

    std::string code = argv[1];

    Controller controller(std::move(code));

    return 0;
}
