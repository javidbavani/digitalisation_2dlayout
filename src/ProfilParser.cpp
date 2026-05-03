#include "../include/ProfileParser.h"
#include <fstream>
#include <sstream>

nlohmann::json parseProfile(const fs::path &profileFile)
{
    std::ifstream file(profileFile);
    if (!file.is_open())
        throw std::runtime_error("Cannot open profile: " + profileFile.string());

    nlohmann::json boundary = nlohmann::json::array();
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        char type;
        iss >> type;

        if (type == 'L')
        {
            // L  x1 y1  x2 y2  sym polarity
            double x1, y1, x2, y2;
            iss >> x1 >> y1 >> x2 >> y2;
            boundary.push_back({{"type", "line"},
                                {"x1", x1},
                                {"y1", y1},
                                {"x2", x2},
                                {"y2", y2}});
        }
        else if (type == 'A')
        {
            // A  x1 y1  x2 y2  cx cy  sym polarity cw
            double x1, y1, x2, y2, cx, cy;
            int cw = 0;
            iss >> x1 >> y1 >> x2 >> y2 >> cx >> cy;
            iss >> cw; // skip sym + polarity first if needed
            boundary.push_back({{"type", "arc"},
                                {"x1", x1},
                                {"y1", y1},
                                {"x2", x2},
                                {"y2", y2},
                                {"cx", cx},
                                {"cy", cy},
                                {"cw", cw == 1}});
        }
    }
    return {{"boundary", boundary}};
}