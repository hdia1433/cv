#pragma once
#include "pch.hpp"
#include "nodes.hpp"

class Generator
{
private:
    const nodes::Node root;

public:
    Generator(nodes::Node);

    std::string generate();
};