#pragma once
#include "pch.hpp"
#include "node.hpp"

class IRGenerator
{
private:

public:
    IRGenerator();

    void generate(const std::vector<nodes::Node*>& ast);

    void print();
private:
};