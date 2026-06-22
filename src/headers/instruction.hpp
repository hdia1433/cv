#pragma once
#include "symbol.hpp"
#include <cstddef>
#include <variant>

enum class OpCode
{
    functionBegin,
    functionEnd,
    define,
    plus,
    minus,
    mult,
    assign,
    abort
};

enum class OperandKind
{
    symbol,
    reference,
    deReference,
    temporary,
    tempAddress,
    immediate
};

using Immediate = std::variant<int, char>;

struct Temporary
{
    int value;

    bool operator==(const Temporary& other) const;
};

struct Operand
{
    OperandKind kind;

    std::variant<Symbol*, Temporary, Immediate> value;

  private: 
    enum class Operation
    {
        Plus,
        Minus,
        Mult
    };

  public:
    bool operator==(const Operand& other) const;
    Operand operator+(const Operand& other) const;
    Operand operator-(const Operand& other) const;
    Operand operator*(const Operand& other) const;
    Operand operation(Operation op, const Operand& other) const;
};

template <> struct std::hash<Operand>
{
    std::size_t operator()(const Operand& op) const
    {
        std::size_t h1 = std::hash<OperandKind>{}(op.kind);
        std::size_t h2;

        switch (op.kind) {
            case OperandKind::symbol:
            case OperandKind::reference:
            case OperandKind::deReference:
                h2 = std::hash<Symbol*>{}(std::get<Symbol*>(op.value));
                break;
            case OperandKind::tempAddress:
            case OperandKind::temporary:
                h2 = std::hash<int>{}(std::get<Temporary>(op.value).value);
                break;
            case OperandKind::immediate:
                h2 = std::hash<Immediate>{}(std::get<Immediate>(op.value));
                break;
        }

        return h1 ^ (h2 << 1);
    }
};

struct Instruction
{
    OpCode operation;
    Operand result;
    Operand arg1;
    Operand arg2;

    std::string toString();

  private:
    std::string opToString(const Operand& op);
};
