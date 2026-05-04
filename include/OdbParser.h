#pragma once
#include <filesystem>
#include <../nlohmann/json.hpp>

namespace fs = std::filesystem;

class OdbParser
{
public:
    explicit OdbParser(const fs::path &odbRoot);
    nlohmann::json parse();

private:
    fs::path m_root;

    fs::path stepPath() const; // resolves steps/<name>/

    nlohmann::json parseStackup();
};