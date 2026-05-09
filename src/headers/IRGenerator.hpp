#pragma once
#include "pch.hpp"
#include "node.hpp"
#include "instruction.hpp"
#include "node.hpp"
#include <unordered_map>

class IRGenerator
{
private:
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, Variable*>* currentSymbolTable;
    uint tempNum;
    uint offset;
public:
    IRGenerator();

    std::vector<Instruction>& getInstructions();

    void generate(const std::vector<nodes::Node*>& ast);

    void printToFile(const std::string& fileName = "irg.cvirg");
private:
    void generate(nodes::FuncDecl* funcDecl);
    void generate(nodes::Abort* abort);

    Operand generateBinary(nodes::Binary* binary);
    void generateBody(nodes::Node* node);
    Operand generateVarDecl(nodes::VarDecl* varDecl);
    Operand generateVarRef(nodes::VarRef* varRef);
    Operand generateExpression(nodes::Node* node);
};