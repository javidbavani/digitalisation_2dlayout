#include "../include/OdbParser.h"
#include "../include/MatrixParser.h"
#include <iostream>

OdbParser::OdbParser(const fs::path &odbRoot) : m_root(odbRoot)
// Validate the root path, sanity checks
{
    if (!fs::exists(odbRoot))
        throw std::runtime_error("ODB++ root not found: " + odbRoot.string());
}

fs::path OdbParser::stepPath() const
{
    fs::path steps = m_root / "steps";
    for (auto &entry : fs::directory_iterator(steps))
        if (entry.is_directory())
            return entry.path();
    throw std::runtime_error("No step directory found under steps/");
}

nlohmann::json OdbParser::parseStackup()
{
    return ::parseMatrix(m_root / "matrix" / "matrix");
}

nlohmann::json OdbParser::parse()
{
    std::cout << "Parsing stackup from matrix...\n";
    auto stackup = parseStackup();


    return {
        {"stackup", stackup}
    };
}