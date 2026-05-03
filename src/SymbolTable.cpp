#include "../include/SymbolTable.h"
#include <sstream>

void SymbolTable::add(int index, const std::string &def)
{
    std::istringstream iss(def);
    std::string typeName;
    iss >> typeName;

    Symbol sym;
    if (typeName == "round")
    {
        sym.type = SymbolType::Round;
        iss >> sym.p1;
    }
    else if (typeName == "rect")
    {
        sym.type = SymbolType::Rect;
        iss >> sym.p1 >> sym.p2;
    }
    else if (typeName == "oval" ||
             typeName == "oblong")
    {
        sym.type = SymbolType::Oval;
        iss >> sym.p1 >> sym.p2;
    }
    else
    {
        sym.type = SymbolType::Unknown;
    }

    m_symbols[index] = sym;
}

const Symbol &SymbolTable::get(int index) const
{
    auto it = m_symbols.find(index);
    return (it != m_symbols.end()) ? it->second : m_unknown;
}