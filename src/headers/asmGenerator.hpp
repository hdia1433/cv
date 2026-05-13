#pragma once
#include "pch.hpp"
#include "instruction.hpp"
#include <sstream>

enum class SectionType
{
    text,
    data,
    bss
};

class AsmGenerator
{
private:
    std::stringstream assembly;
    std::vector<Instruction>* irCode;
    int indentNum;
    uint index;
    uint regNum;
    SectionType currentSection;
public:
    AsmGenerator();

    std::string getAssembly();

    void generate(std::vector<Instruction>& irCode);
private:
    //Structures
    void generateFunctionDecl(bool init = false);
    void generatePrologue(int localSize);
    void generateEpilogue(int localSize);
    void generateGlobalVariable();
    void generateGlobalVariable(std::variant<int> value);
    void generateStaticInit(std::vector<Instruction>&& instructions);

    //keywords
    void generateAbort();

    //Expression tree
    void generatePlus();
    void generatePlus(const Instruction& instr);
    void generateMinus();
    void generateMinus(const Instruction& instr);
    void generateMult();
    void generateMult(const Instruction& instr);
    void generateAssign();
    void generateAssign(const Instruction& instr);

    void accessVar(Variable* symbol, const std::string& reg = "w28");
    void setVar(Variable* symbol, const std::string& useReg = "w27", const std::string& storeReg = "w28");
    void generateOperation(const std::string& operation);
    void generateOperation(const std::string& operation, const Instruction& instr);
    Instruction* peek();
    Instruction consume();
    std::string getReg(int temp);
    int align16(int value);
};