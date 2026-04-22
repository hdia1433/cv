#pragma once
#include "pch.hpp"
#include "instruction.hpp"
#include <sstream>

class AsmGenerator
{
private:
    std::stringstream assembly;
    std::vector<Instruction>* irCode;
    int indentNum;
    uint index;
public:
    AsmGenerator();

    std::string getAssembly();

    void generate(std::vector<Instruction>& irCode);
private:
    //Structures
    void generateFunctionDecl();
    void generatePrologue();
    void generateEpilogue();

    //keywords
    void generateAbort();

    //Expression tree
    void generatePlus();

    Instruction* peek();
    Instruction consume();
    std::string getReg(const std::string& temp);
};