#include "include\MatrixParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

nlohmann::json parseMatrix(const fs::path& matrixFile) {
    std::ifstream file(matrixFile);
    if (!file.is_open())
        throw std::runtime_error("Cannot open matrix file: "
                                  + matrixFile.string());

    nlohmann::json stackup = nlohmann::json::array();
    std::string line;
    double z = 0.0;   //accumulate Z from bottom up

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        // ODB++ matrix rows start with LAYER
        if (token != "LAYER") continue;

        Layer L;
        std::string kvPair;
        while (iss >> kvPair) {
            auto eq = kvPair.find('=');
            if (eq == std::string::npos) continue;
            std::string key = kvPair.substr(0, eq);
            std::string val = kvPair.substr(eq + 1);

            if (key == "NAME")      L.name = val;
            else if (key == "TYPE") L.type = val;
            else if (key == "THICKNESS")
                L.thickness_mm = std::stod(val);
        }

        double z_start = z;
        z += L.thickness_mm;

        stackup.push_back({
            {"name",         L.name},
            {"type",         L.type},
            {"z_start_mm",   z_start},
            {"z_end_mm",     z},
            {"thickness_mm", L.thickness_mm}
        });
    }
    return stackup;
}