#pragma once
#include <string>
#include <unordered_map>

enum class SymbolType
{
    Round,
    Rect,
    Oval,
    Unknown
};

struct Symbol
{
    SymbolType type = SymbolType::Unknown;
    double p1 = 0; // diameter  OR  width
    double p2 = 0; // height (rect / oval only)
};

class SymbolTable
{
public:
    void add(int index, const std::string &def);
    const Symbol &get(int index) const;

private:
    std::unordered_map<int, Symbol> m_symbols;
    Symbol m_unknown;
};