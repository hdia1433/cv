#include "pch.hpp"
#include "controller.hpp"
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        throw std::runtime_error("An error has occurred. A file was expected to be passed. cv <file>");
    }

    std::string fileName = argv[1];

    std::fstream file(fileName);

    if(!file.is_open())
    {
        std::stringstream error;
        error << "An error has occurred. The file provided, '" << fileName << "', couldn't be opened.";
        throw std::runtime_error(error.str());
    }

    std::stringstream codestream;

    codestream << file.rdbuf();

    file.close();

    std::string code = codestream.str();

    Controller controller(std::move(code));
    controller.start();

    return 0;
}
