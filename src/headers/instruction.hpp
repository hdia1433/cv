#pragma once
#include "symbol.hpp"
#include <functional>
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
    store,
    abort
};

enum class OperandKind
{
    symbol,
    reference,
    temporary,
    immediate
};

struct Operand
{
    OperandKind kind;

    union
    {
        Symbol* symbol;
        int temporary;
        std::variant<int, char> immediate;
    };

    bool operator==(const Operand& other) const;
    Operand operator+(const Operand& other) const;
    Operand operator-(const Operand& other) const;
    Operand operator*(const Operand& other) const;

  private:
    Operand operation(const Operand& other, const std::string& op) const;
};

template <> struct std::hash<Operand>
{
    std::size_t operator()(const Operand& op) const
    {
        std::size_t h1 = std::hash<OperandKind>{}(op.kind);
        std::size_t h2;

        switch (op.kind)
        {
            case OperandKind::symbol:
            case OperandKind::reference:
                h2 = std::hash<Symbol*>{}(op.symbol);
                break;
            case OperandKind::temporary:
                h2 = std::hash<int>{}(op.temporary);
                break;
            case OperandKind::immediate:
                h2 = std::hash<std::variant<int, char>>{}(op.immediate);
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
