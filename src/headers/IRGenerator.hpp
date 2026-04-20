#pragma once
#include "pch.hpp"
#include "node.hpp"
#include "instruction.hpp"
#include "node.hpp"

class IRGenerator
{
private:
    std::vector<Instruction> instructions;
public:
    IRGenerator();

    std::vector<Instruction>& getInstructions();

    void generate(const std::vector<nodes::Node*>& ast);

    void printToFile(const std::string& fileName = "irg.cvirg");
private:
    void generate(nodes::FuncDecl* funcDecl);
    void generate(nodes::Abort* abort);

    void generateBody(nodes::Node* node);
    std::string generateExpression(nodes::Node* node);
};