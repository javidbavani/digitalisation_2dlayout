#pragma once
#include <filesystem>
#include <../nlohmann/json.hpp>

namespace fs = std::filesystem;

nlohmann::json parseProfile(const fs::path &profileFile);