#pragma once
#include "pch.hpp"
#include "token.hpp"
#include <unordered_map>
#include "token.hpp"
#include "functional"
#include "type.hpp"

struct FunctionSignature
{
    std::string name;
    Type returnType;

    bool operator==(const FunctionSignature& other) const;
};

template<>
struct std::hash<FunctionSignature>
{
    std::size_t operator()(const FunctionSignature& sig) const
    {
        std::size_t h1 = std::hash<std::string>{}(sig.name);
        std::size_t h2 = std::hash<Type>{}(sig.returnType);

        return h1 ^ (h2 << 1);
    }
};

enum class SymbolType
{
    var,
    func
};

struct Symbol
{
    SymbolType kind;

    Symbol(SymbolType kind);

    virtual ~Symbol() = default;
};

struct Variable: public Symbol
{
    uint offset;
    std::string name;
    Type type;
    Coordinate location;
    bool global;
    bool used;

    Variable(const std::string& name, Type type, const Coordinate& location);
};

struct Function: public Symbol
{
    std::string name;
    Type returnType;
    std::unordered_map<std::string, Variable*> locals;
    Coordinate location;

    Function(const std::string& name, Type returnType, const Coordinate& location);
    ~Function();
};

struct GlobalScope
{
    std::unordered_map<std::string, Variable*> variables;
    std::unordered_map<FunctionSignature, Function*> functions;

    GlobalScope();
    ~GlobalScope();
};

GlobalScope& getGlobalScope();