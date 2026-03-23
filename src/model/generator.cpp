#include "generator.hpp"
#include "sstream"

Generator::Generator(nodes::Node root):
root(std::move(root))
{

}

std::string Generator::generate()
{
    return "";
}