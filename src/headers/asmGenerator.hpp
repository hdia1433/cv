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
    uint regNum;
public:
    AsmGenerator();

    std::string getAssembly();

    void generate(std::vector<Instruction>& irCode);
private:
    //Structures
    void generateFunctionDecl();
    void generatePrologue(int localSize);
    void generateEpilogue(int localSize);

    //keywords
    void generateAbort();

    //Expression tree
    void generatePlus();
    void generateAssign();

    Instruction* peek();
    Instruction consume();
    std::string getReg(int temp);
    int align16(int value);
};