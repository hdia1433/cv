#pragma once
#include "pch.hpp"
#include "nodes.hpp"

class Generator
{
private:
    const std::vector<std::unique_ptr<nodes::Node>> root;

public:
    Generator(std::vector<std::unique_ptr<nodes::Node>>);

    std::string generate();
};