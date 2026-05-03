#pragma once
#include <filesystem>
#include <../nlohmann/json.hpp>
#include "SymbolTable.h"

namespace fs = std::filesystem;

class FeaturesParser
{
public:
    nlohmann::json parse(const fs::path &featuresFile,
                         const std::string &layerName);

private:
    SymbolTable m_symbols;

    void parseSymbolTable(std::istream &in);
    nlohmann::json parsePad(std::istringstream &iss);
    nlohmann::json parseLine(std::istringstream &iss);
    nlohmann::json parseArc(std::istringstream &iss);
    nlohmann::json parseSurface(std::istream &in,
                                std::istringstream &firstLine);
};