#pragma once
#include "pch.hpp"
#include "nodes.hpp"

class Generator
{
private:
    const std::vector<std::unique_ptr<nodes::Node>> nodes;
    int indentNum;

public:
    Generator(std::vector<std::unique_ptr<nodes::Node>>);

    std::string generate();

private:
    //structures
    std::string generateFunc(nodes::FunctionDecl*);

    //keywords
    std::string generateExit(nodes::Exit*);
    std::string generateInt(nodes::Int*);

    //blocks
    std::string generateGlobal(const std::vector<std::unique_ptr<nodes::Node>>&);
    std::string generateBlock(const std::vector<std::unique_ptr<nodes::Node>>&);

    //helpers
    std::string nodeToFunc(nodes::Node*);
};