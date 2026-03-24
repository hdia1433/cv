#include "generator.hpp"
#include "sstream"

Generator::Generator(std::vector<std::unique_ptr<nodes::Node>> root):
root(std::move(root))
{

}

std::string Generator::generate()
{
    std::stringstream assembly;

    

    return assembly.str();
}