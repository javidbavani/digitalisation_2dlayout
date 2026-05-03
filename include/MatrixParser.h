#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <../nlohmann/json.hpp>

namespace fs = std::filesystem;

struct Layer
{
    std::string name;
    std::string type; // COPPER, DIELECTRIC, DRILL, MASK ...
    int index = 0;
    double thickness_mm = 0.0;
};

nlohmann::json parseMatrix(const fs::path &matrixFile);